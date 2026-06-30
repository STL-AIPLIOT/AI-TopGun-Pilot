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

#ifndef DECORATOR_REPEAT_NODE_H
#define DECORATOR_REPEAT_NODE_H

// DecoratorNode 관련 정의를 가져온다.
// RepeatNode는 자식 노드 하나를 감싸서 여러 번 반복 실행하는 데코레이터 노드이므로
// DecoratorNode를 상속받기 위해 이 헤더가 필요하다.
#include "../decorator_node.h"

namespace BT
{

/**
 * RepeatNode
 *
 * 자식 노드를 지정한 횟수만큼 반복 실행하는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 같은 행동을 여러 번 반복해야 할 때 사용한다.
 * - 자식 노드가 성공하면 반복 횟수를 증가시키고 다시 실행한다.
 * - 지정된 횟수만큼 성공하면 RepeatNode도 SUCCESS를 반환한다.
 *
 * 동작 방식:
 * - 자식 노드가 SUCCESS를 반환하면 반복 횟수를 1 증가시킨다.
 * - 자식 노드가 num_cycles 횟수만큼 SUCCESS를 반환하면 최종 SUCCESS를 반환한다.
 * - 자식 노드가 FAILURE를 반환하면 반복을 중단하고 FAILURE를 반환한다.
 * - 자식 노드가 RUNNING이면 아직 실행 중이므로 RUNNING을 반환한다.
 *
 * 사용 예시:
 *
 * <Repeat num_cycles="3">
 *   <ClapYourHandsOnce/>
 * </Repeat>
 *
 * 위 예시는 ClapYourHandsOnce 노드를 3번 성공할 때까지 반복 실행한다.
 */
class RepeatNode : public DecoratorNode
{
  public:

    /**
     * 생성자 1
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * NTries:
     * - 자식 노드를 몇 번 성공할 때까지 반복할지 정하는 값이다.
     *
     * 예:
     * - NTries = 3이면 자식 노드가 SUCCESS를 3번 반환해야
     *   RepeatNode도 SUCCESS를 반환한다.
     */
    RepeatNode(const std::string& name, int NTries);

    /**
     * 생성자 2
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * config:
     * - XML 포트 정보와 Blackboard 연결 정보를 담고 있는 설정 객체이다.
     *
     * 이 생성자는 반복 횟수를 코드에서 직접 받지 않고,
     * XML의 num_cycles 포트를 통해 받을 때 사용한다.
     */
    RepeatNode(const std::string& name, const NodeConfiguration& config);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~RepeatNode() override = default;

    /**
     * providedPorts()
     *
     * RepeatNode가 XML에서 사용할 수 있는 포트 목록을 정의한다.
     *
     * num_cycles:
     * - 자식 노드를 몇 번 성공할 때까지 반복할지 나타내는 값이다.
     * - -1을 사용하면 무한 반복을 의미한다.
     *
     * 사용 예시:
     * <Repeat num_cycles="3">
     *     ...
     * </Repeat>
     */
    static PortsList providedPorts()
    {
        return {
            InputPort<int>(
                NUM_CYCLES,
                "Repeat a succesful child up to N times. "
                "Use -1 to create an infinite loop."
            )
        };
    }

  private:
    /**
     * num_cycles_
     *
     * 자식 노드를 몇 번 성공할 때까지 반복할지 저장하는 변수이다.
     *
     * 예:
     * - num_cycles_ = 3이면 자식 노드가 SUCCESS를 3번 반환해야
     *   RepeatNode가 SUCCESS를 반환한다.
     *
     * 특수 값:
     * - num_cycles_ = -1이면 무한 반복을 의미한다.
     */
    int num_cycles_;

    /**
     * try_index_
     *
     * 현재까지 자식 노드가 성공한 횟수를 저장하는 변수이다.
     *
     * 필요한 이유:
     * - 자식 노드가 몇 번 SUCCESS를 반환했는지 기억해야
     *   반복 종료 시점을 판단할 수 있다.
     */
    int try_index_;

    /**
     * read_parameter_from_ports_
     *
     * num_cycles 값을 포트에서 읽어야 하는지 판단하는 변수이다.
     *
     * true:
     * - XML 또는 Blackboard 포트에서 num_cycles 값을 읽는다.
     *
     * false:
     * - 생성자에서 직접 받은 NTries 값을 사용한다.
     */
    bool read_parameter_from_ports_;

    /**
     * NUM_CYCLES
     *
     * 반복 횟수 포트 이름을 저장한 상수이다.
     *
     * 문자열 "num_cycles"를 여러 곳에 직접 쓰지 않고,
     * 하나의 상수로 관리하기 위해 사용한다.
     */
    static constexpr const char* NUM_CYCLES = "num_cycles";

    /**
     * tick()
     *
     * Behavior Tree가 RepeatNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 필요한 경우 포트에서 num_cycles 값을 읽는다.
     * 2. 자식 노드를 실행한다.
     * 3. 자식이 SUCCESS이면 성공 횟수를 증가시킨다.
     * 4. 성공 횟수가 num_cycles_에 도달하면 SUCCESS를 반환한다.
     * 5. 자식이 FAILURE이면 반복을 중단하고 FAILURE를 반환한다.
     * 6. 자식이 RUNNING이면 RUNNING을 반환한다.
     */
    virtual NodeStatus tick() override;

    /**
     * halt()
     *
     * RepeatNode 실행을 중지할 때 호출된다.
     *
     * 사용 목적:
     * - 실행 중인 자식 노드를 중지한다.
     * - 반복 횟수 try_index_를 초기화해서
     *   다음 실행 때 처음부터 다시 반복할 수 있게 한다.
     */
    void halt() override;
};

} // namespace BT

#endif