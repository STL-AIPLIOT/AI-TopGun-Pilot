/* Copyright (C) 2015-2018 Michele Colledanchise -  All Rights Reserved
 * Copyright (C) 2018-2020 Davide Faconti, Eurecat -  All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PARALLEL_NODE_H
#define PARALLEL_NODE_H

// 여러 자식 노드의 성공/실패 개수를 판단할 때 사용한다.
// 이미 SUCCESS 또는 FAILURE가 확정된 자식 노드를 skip_list_에 저장하기 위해 필요하다.
#include <set>

// ControlNode 관련 정의를 가져온다.
// ParallelNode는 여러 자식 노드를 관리하는 제어 노드이므로 ControlNode를 상속받는다.
#include "../control_node.h"

namespace BT
{

/**
 * ParallelNode
 *
 * 여러 자식 노드를 실행하면서,
 * 정해진 성공 개수(threshold)에 도달하면 SUCCESS를 반환하는 제어 노드이다.
 *
 * 사용 목적:
 * - 여러 행동이나 조건을 동시에 관리해야 할 때 사용한다.
 * - 모든 자식이 성공할 필요는 없고, 일정 개수 이상만 성공하면 전체 성공으로 판단한다.
 *
 * 예시:
 * - 자식 노드가 5개이고 threshold가 3이면,
 *   5개 중 3개 이상이 SUCCESS를 반환했을 때 ParallelNode도 SUCCESS를 반환한다.
 *
 * 주의:
 * - 실제 멀티스레드처럼 완전히 동시에 실행된다는 뜻은 아니다.
 * - Behavior Tree의 tick 흐름 안에서 여러 자식 노드의 상태를 함께 관리하는 구조이다.
 */
class ParallelNode : public ControlNode
{
  public:

    /**
     * 생성자 1
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * threshold:
     * - 몇 개의 자식 노드가 성공해야 ParallelNode가 SUCCESS를 반환할지 정하는 값이다.
     *
     * 예:
     * - threshold = 2이면 자식 노드 중 2개 이상이 SUCCESS일 때 전체 SUCCESS가 된다.
     */
    ParallelNode(const std::string& name, unsigned threshold);

    /**
     * 생성자 2
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * config:
     * - XML 포트, Blackboard 연결 정보 등 노드 실행에 필요한 설정 정보를 담고 있다.
     *
     * 이 생성자는 threshold 값을 코드에서 직접 받지 않고,
     * 포트 또는 XML 설정을 통해 받을 때 사용한다.
     */
    ParallelNode(const std::string& name, const NodeConfiguration& config);

    /**
     * providedPorts()
     *
     * ParallelNode가 XML에서 사용할 수 있는 포트 목록을 정의한다.
     *
     * threshold:
     * - ParallelNode가 SUCCESS를 반환하기 위해 필요한 성공 자식 노드 개수이다.
     *
     * 사용 예시:
     * <Parallel threshold="2">
     *     ...
     * </Parallel>
     */
    static PortsList providedPorts()
    {
        return { InputPort<unsigned>(THRESHOLD_KEY) };
    }

    /**
     * 소멸자
     *
     * 특별한 자원 해제 작업이 없으므로 기본 소멸자를 사용한다.
     */
    ~ParallelNode() = default;

    /**
     * halt()
     *
     * ParallelNode 실행을 중지할 때 호출된다.
     *
     * 사용 목적:
     * - 실행 중이던 자식 노드들을 중단한다.
     * - skip_list_를 초기화해서 다음 실행 때 다시 모든 자식을 검사할 수 있게 한다.
     */
    virtual void halt() override;

    /**
     * thresholdM()
     *
     * 현재 설정된 threshold 값을 반환한다.
     *
     * threshold는 ParallelNode가 SUCCESS로 판단하기 위해 필요한
     * 성공 자식 노드의 개수이다.
     */
    unsigned int thresholdM();

    /**
     * setThresholdM()
     *
     * ParallelNode의 threshold 값을 새로 설정한다.
     *
     * threshold_M:
     * - 새로 설정할 성공 기준 개수이다.
     */
    void setThresholdM(unsigned int threshold_M);

  private:
    /**
     * threshold_
     *
     * ParallelNode가 SUCCESS를 반환하기 위해 필요한 성공 자식 노드 개수이다.
     *
     * 예:
     * - 자식이 4개이고 threshold_가 2이면,
     *   2개 이상이 SUCCESS일 때 ParallelNode도 SUCCESS를 반환한다.
     */
    unsigned int threshold_;

    /**
     * skip_list_
     *
     * 이미 결과가 확정된 자식 노드의 인덱스를 저장하는 집합이다.
     *
     * 필요한 이유:
     * - 이미 SUCCESS 또는 FAILURE가 확정된 자식 노드를
     *   매 tick마다 다시 실행하지 않기 위해 사용한다.
     * - 불필요한 중복 실행을 줄이고 현재 RUNNING 중인 노드에 집중할 수 있다.
     */
    std::set<int> skip_list_;

    /**
     * read_parameter_from_ports_
     *
     * threshold 값을 포트에서 읽어야 하는지 판단하는 변수이다.
     *
     * true:
     * - XML 또는 Blackboard 포트에서 threshold 값을 읽는다.
     *
     * false:
     * - 생성자에서 직접 받은 threshold 값을 사용한다.
     */
    bool read_parameter_from_ports_;

    /**
     * THRESHOLD_KEY
     *
     * threshold 포트 이름을 상수로 저장한다.
     *
     * 문자열 "threshold"를 여러 곳에 직접 쓰지 않고,
     * 하나의 상수로 관리하기 위해 사용한다.
     */
    static constexpr const char* THRESHOLD_KEY = "threshold";

    /**
     * tick()
     *
     * Behavior Tree가 ParallelNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 필요한 경우 포트에서 threshold 값을 읽는다.
     * 2. 자식 노드들을 실행한다.
     * 3. SUCCESS 개수를 센다.
     * 4. FAILURE 개수를 센다.
     * 5. SUCCESS 개수가 threshold_ 이상이면 SUCCESS를 반환한다.
     * 6. 더 이상 threshold_에 도달할 수 없으면 FAILURE를 반환한다.
     * 7. 아직 판단이 끝나지 않았으면 RUNNING을 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

}

#endif   // PARALLEL_NODE_H