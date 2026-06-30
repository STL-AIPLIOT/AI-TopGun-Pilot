#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

// 여러 스레드에서 공유 자원에 동시에 접근하지 못하도록 보호할 때 사용한다.
#include <mutex>

// 특정 조건이 만족될 때까지 스레드를 잠시 기다리게 할 때 사용한다.
#include <condition_variable>

// 타이머 작업을 별도의 작업 스레드에서 실행하기 위해 사용한다.
#include <thread>

// 타이머 작업들을 우선순위 큐로 관리하기 위해 사용한다.
#include <queue>

// 시간 계산을 위해 사용한다.
// 예: milliseconds, steady_clock 등
#include <chrono>

// 프로그램 내부 상태가 맞는지 확인하기 위해 사용한다.
#include <assert.h>

// std::function, std::greater 사용을 위해 필요하다.
// 기존 코드에 없으면 컴파일 환경에 따라 에러가 날 수 있다.
#include <functional>

// std::vector 사용을 명확히 하기 위해 추가하는 것이 안전하다.
#include <vector>

namespace BT
{

// TimerQueue 내부에서만 사용하는 보조 기능들을 모아둔 namespace이다.
namespace details
{

/**
 * Semaphore
 *
 * 스레드 사이에서 신호를 주고받기 위한 간단한 세마포어 클래스이다.
 *
 * 사용 목적:
 * - TimerQueue의 작업 스레드가 기다리고 있다가,
 *   새로운 타이머가 추가되거나 취소되면 깨어나도록 만들기 위해 사용한다.
 *
 * 쉽게 말하면:
 * - notify()는 "일어나서 확인해라"라는 신호를 보내는 함수이다.
 * - wait()는 신호가 올 때까지 기다리는 함수이다.
 */
class Semaphore
{
  public:
    /**
     * 생성자
     *
     * count:
     * - 처음부터 몇 개의 신호를 가지고 시작할지 정하는 값이다.
     * - 기본값은 0이므로, 처음에는 wait()가 호출되면 대기한다.
     */
    Semaphore(unsigned int count = 0) : m_count(count)
    {
    }

    /**
     * notify()
     *
     * 대기 중인 스레드 하나를 깨우는 함수이다.
     *
     * 동작:
     * - m_count를 1 증가시킨다.
     * - condition_variable을 통해 기다리는 스레드 하나를 깨운다.
     */
    void notify()
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_count++;
        m_cv.notify_one();
    }

    /**
     * wait()
     *
     * 신호가 올 때까지 기다리는 함수이다.
     *
     * 동작:
     * - m_count가 0이면 계속 기다린다.
     * - notify()가 호출되어 m_count가 증가하면 대기에서 깨어난다.
     * - 깨어난 뒤 m_count를 1 감소시킨다.
     */
    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv.wait(lock, [this]() { return m_count > 0; });
        m_count--;
    }

    /**
     * waitUntil()
     *
     * 특정 시간까지 신호가 오는지 기다리는 함수이다.
     *
     * point:
     * - 기다릴 종료 시간이다.
     *
     * 반환:
     * - 시간 안에 notify() 신호를 받으면 true
     * - 시간이 지날 때까지 신호를 받지 못하면 false
     */
    template <class Clock, class Duration>
    bool waitUntil(const std::chrono::time_point<Clock, Duration>& point)
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        // point 시간까지 m_count가 0보다 커지는지 기다린다.
        if (!m_cv.wait_until(lock, point, [this]() { return m_count > 0; }))
            return false;

        m_count--;
        return true;
    }

  private:
    // 여러 스레드가 m_count에 동시에 접근하지 못하도록 보호한다.
    std::mutex m_mtx;

    // 스레드를 기다리게 하거나 깨우는 데 사용한다.
    std::condition_variable m_cv;

    // 현재 사용 가능한 신호 개수를 저장한다.
    unsigned int m_count;
};

} // namespace details

/**
 * TimerQueue
 *
 * 미래의 특정 시간에 함수를 실행하기 위한 타이머 큐 클래스이다.
 *
 * 사용 목적:
 * - 일정 시간이 지난 뒤 특정 handler 함수를 실행하고 싶을 때 사용한다.
 * - TimeoutNode에서 자식 노드가 제한 시간을 넘겼는지 확인할 때 사용된다.
 *
 * 보장 사항:
 * - 등록된 handler는 한 번 실행된다.
 * - cancel된 handler도 실행은 되지만, aborted 인자가 true로 전달된다.
 * - TimerQueue가 삭제되면 모든 타이머를 취소한다.
 * - handler는 TimerQueue 내부 작업 스레드에서 실행된다.
 *
 * 주의:
 * - handler 실행 순서는 보장되지 않는다.
 */
class TimerQueue
{
  public:
    /**
     * 생성자
     *
     * TimerQueue 객체가 생성되면 내부 작업 스레드를 시작한다.
     *
     * 이 스레드는 run() 함수를 실행하면서
     * 등록된 타이머가 만료되었는지 계속 확인한다.
     */
    TimerQueue()
    {
        m_th = std::thread([this] { run(); });
    }

    /**
     * 소멸자
     *
     * TimerQueue가 사라질 때 모든 타이머를 취소하고,
     * 작업 스레드를 안전하게 종료한다.
     */
    ~TimerQueue()
    {
        // 남아 있는 모든 타이머를 취소한다.
        cancelAll();

        // 0ms짜리 타이머를 추가해서 작업 스레드를 깨우고 종료 플래그를 설정한다.
        add(std::chrono::milliseconds(0), [this](bool) { m_finish = true; });

        // 작업 스레드가 완전히 끝날 때까지 기다린다.
        m_th.join();
    }

    /**
     * add()
     *
     * 새로운 타이머를 추가하는 함수이다.
     *
     * milliseconds:
     * - 몇 ms 뒤에 handler를 실행할지 나타낸다.
     *
     * handler:
     * - 시간이 되었을 때 실행할 함수이다.
     * - bool 인자는 취소 여부를 의미한다.
     * - true이면 취소된 상태로 실행된 것이다.
     * - false이면 정상적으로 시간이 지나 실행된 것이다.
     *
     * 반환:
     * - 새로 등록된 타이머의 ID
     * - 이 ID를 이용해서 나중에 cancel() 할 수 있다.
     */
    uint64_t add(std::chrono::milliseconds milliseconds, std::function<void(bool)> handler)
    {
        WorkItem item;

        // 현재 시간으로부터 milliseconds 이후가 실행 시점이다.
        item.end = Clock::now() + milliseconds;

        // handler를 WorkItem 안으로 이동시킨다.
        item.handler = std::move(handler);

        std::unique_lock<std::mutex> lk(m_mtx);

        // 타이머 ID를 하나 증가시켜 새 ID로 사용한다.
        uint64_t id = ++m_idcounter;
        item.id = id;

        // 우선순위 큐에 타이머 작업을 넣는다.
        m_items.push(std::move(item));

        lk.unlock();

        // 새 타이머가 들어왔으므로 작업 스레드를 깨운다.
        m_checkWork.notify();

        return id;
    }

    /**
     * cancel()
     *
     * 특정 ID를 가진 타이머를 취소하는 함수이다.
     *
     * id:
     * - 취소할 타이머 ID이다.
     *
     * 반환:
     * - 1: 취소 성공
     * - 0: 이미 실행되었거나, 존재하지 않는 ID라서 취소 실패
     *
     * 특징:
     * - 큐에서 직접 삭제하지 않는다.
     * - 기존 item의 handler를 비우고,
     *   취소된 handler를 즉시 실행될 새 item으로 옮긴다.
     */
    size_t cancel(uint64_t id)
    {
        std::unique_lock<std::mutex> lk(m_mtx);

        // 큐 내부 컨테이너를 순회하면서 해당 ID의 타이머를 찾는다.
        for (auto&& item : m_items.getContainer())
        {
            if (item.id == id && item.handler)
            {
                WorkItem newItem;

                // 즉시 실행되도록 가장 빠른 시간으로 설정한다.
                newItem.end = Clock::time_point();

                // id가 0이면 취소된 타이머라는 의미이다.
                newItem.id = 0;

                // 기존 handler를 새 item으로 옮긴다.
                newItem.handler = std::move(item.handler);

                // 기존 item의 handler를 비워서 나중에 실행되지 않게 한다.
                item.handler = nullptr;

                // 취소된 handler를 즉시 실행되도록 큐에 넣는다.
                m_items.push(std::move(newItem));

                lk.unlock();

                // 큐 상태가 바뀌었으므로 작업 스레드를 깨운다.
                m_checkWork.notify();

                return 1;
            }
        }

        return 0;
    }

    /**
     * cancelAll()
     *
     * 등록된 모든 타이머를 취소하는 함수이다.
     *
     * 반환:
     * - 취소 대상으로 처리된 타이머 개수
     */
    size_t cancelAll()
    {
        std::unique_lock<std::mutex> lk(m_mtx);

        // 모든 item을 즉시 실행되도록 만들고,
        // id를 0으로 바꿔 취소된 타이머임을 표시한다.
        for (auto&& item : m_items.getContainer())
        {
            if (item.id)
            {
                item.end = Clock::time_point();
                item.id = 0;
            }
        }

        auto ret = m_items.size();

        lk.unlock();

        // 작업 스레드를 깨워 취소된 타이머들을 처리하게 한다.
        m_checkWork.notify();

        return ret;
    }

  private:
    // steady_clock은 시스템 시간 변경의 영향을 덜 받는 시간 측정용 clock이다.
    using Clock = std::chrono::steady_clock;

    // TimerQueue는 복사하면 스레드와 큐 상태가 꼬일 수 있으므로 복사를 금지한다.
    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    /**
     * run()
     *
     * TimerQueue의 내부 작업 스레드가 실행하는 메인 루프이다.
     *
     * 동작:
     * - 타이머가 있으면 가장 빠른 타이머 만료 시간까지 기다린다.
     * - 타이머가 없으면 새 작업이 들어올 때까지 기다린다.
     * - 시간이 된 타이머가 있으면 checkWork()에서 실행한다.
     * - m_finish가 true가 되면 루프를 종료한다.
     */
    void run()
    {
        while (!m_finish)
        {
            auto end = calcWaitTime();

            if (end.first)
            {
                // 실행할 타이머가 있으므로 해당 시간이 될 때까지 기다린다.
                // 중간에 새 타이머가 추가되거나 취소되면 notify()로 깨어날 수 있다.
                m_checkWork.waitUntil(end.second);
            }
            else
            {
                // 타이머가 하나도 없으면 새 작업이 들어올 때까지 무기한 대기한다.
                m_checkWork.wait();
            }

            // 만료된 타이머들을 확인하고 실행한다.
            checkWork();
        }

        // 종료 시점에는 모든 item이 정리되어 있어야 한다.
        assert(m_items.size() == 0);
    }

    /**
     * calcWaitTime()
     *
     * 다음으로 실행해야 할 타이머 시간을 계산하는 함수이다.
     *
     * 반환:
     * - first == true:
     *   실행 대기 중인 타이머가 있으며, second에 가장 빠른 실행 시간이 들어 있다.
     *
     * - first == false:
     *   실행할 타이머가 없다는 의미이다.
     */
    std::pair<bool, Clock::time_point> calcWaitTime()
    {
        std::lock_guard<std::mutex> lk(m_mtx);

        while (m_items.size())
        {
            if (m_items.top().handler)
            {
                // 가장 빨리 실행될 item의 시간을 반환한다.
                return std::make_pair(true, m_items.top().end);
            }
            else
            {
                // handler가 없는 item은 취소된 item이므로 버린다.
                m_items.pop();
            }
        }

        // 큐에 유효한 item이 없으면 기다릴 시간이 없다는 의미이다.
        return std::make_pair(false, Clock::time_point());
    }

    /**
     * checkWork()
     *
     * 시간이 된 타이머 작업들을 실행하는 함수이다.
     *
     * 동작:
     * - 현재 시간보다 end 시간이 작거나 같은 item을 꺼낸다.
     * - item의 handler를 실행한다.
     * - item.id가 0이면 취소된 타이머이므로 handler에 true가 전달된다.
     * - item.id가 0이 아니면 정상 만료된 타이머이므로 false가 전달된다.
     */
    void checkWork()
    {
        std::unique_lock<std::mutex> lk(m_mtx);

        // 큐에 item이 있고, 실행 시간이 된 item이면 계속 꺼내서 실행한다.
        while (m_items.size() && m_items.top().end <= Clock::now())
        {
            WorkItem item(std::move(m_items.top()));
            m_items.pop();

            // handler 실행 중에는 lock을 풀어둔다.
            // handler 내부에서 다시 TimerQueue를 사용할 수도 있기 때문이다.
            lk.unlock();

            if (item.handler)
                item.handler(item.id == 0);

            lk.lock();
        }
    }

    /**
     * m_checkWork
     *
     * 작업 스레드를 깨우기 위한 세마포어이다.
     *
     * 사용 예:
     * - 새 타이머가 추가되었을 때
     * - 기존 타이머가 취소되었을 때
     * - TimerQueue를 종료해야 할 때
     */
    details::Semaphore m_checkWork;

    /**
     * m_th
     *
     * TimerQueue 내부 작업 스레드이다.
     *
     * 이 스레드는 run()을 실행하면서
     * 타이머 만료 여부를 계속 확인한다.
     */
    std::thread m_th;

    /**
     * m_finish
     *
     * 작업 스레드를 종료할지 판단하는 플래그이다.
     *
     * false:
     * - 계속 실행
     *
     * true:
     * - run() 루프 종료
     */
    bool m_finish = false;

    /**
     * m_idcounter
     *
     * 타이머 ID를 생성하기 위한 카운터이다.
     *
     * add()가 호출될 때마다 1씩 증가한다.
     */
    uint64_t m_idcounter = 0;

    /**
     * WorkItem
     *
     * 하나의 타이머 작업 정보를 저장하는 구조체이다.
     */
    struct WorkItem
    {
        // 이 타이머가 실행되어야 하는 시간이다.
        Clock::time_point end;

        // 타이머 ID이다.
        // id == 0이면 취소된 타이머라는 의미이다.
        uint64_t id;

        // 시간이 되었을 때 실행할 함수이다.
        std::function<void(bool)> handler;

        /**
         * operator>()
         *
         * priority_queue에서 실행 시간이 빠른 item이 먼저 나오도록 비교하는 함수이다.
         */
        bool operator>(const WorkItem& other) const
        {
            return end > other.end;
        }
    };

    /**
     * m_mtx
     *
     * 타이머 큐와 관련된 공유 데이터를 보호하는 mutex이다.
     *
     * 보호 대상:
     * - m_items
     * - m_idcounter
     * - 타이머 추가/취소/조회 작업
     */
    std::mutex m_mtx;

    /**
     * Queue
     *
     * priority_queue를 상속해서 내부 컨테이너에 접근할 수 있게 만든 클래스이다.
     *
     * 필요한 이유:
     * - cancel()과 cancelAll()에서 큐 내부의 item들을 직접 확인해야 하기 때문이다.
     *
     * 일반 priority_queue는 내부 컨테이너에 직접 접근할 수 없어서
     * getContainer() 함수를 추가한 것이다.
     */
    class Queue
      : public std::priority_queue<WorkItem, std::vector<WorkItem>, std::greater<WorkItem>>
    {
      public:
        /**
         * getContainer()
         *
         * priority_queue 내부 컨테이너를 반환한다.
         *
         * 사용 목적:
         * - 특정 ID를 가진 타이머를 찾거나,
         *   모든 타이머를 취소 처리하기 위해 사용한다.
         */
        std::vector<WorkItem>& getContainer()
        {
            return this->c;
        }
    } m_items;
};

} // namespace BT

#endif   // TIMERQUEUE_H