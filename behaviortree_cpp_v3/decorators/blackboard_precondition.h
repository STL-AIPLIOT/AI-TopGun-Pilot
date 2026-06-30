/*  Copyright (C) 2018-2020 Davide Faconti, Eurecat -  All Rights Reserved
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

#ifndef DECORATOR_BLACKBOARD_PRECONDITION_NODE_H
#define DECORATOR_BLACKBOARD_PRECONDITION_NODE_H

// DecoratorNode 관련 정의를 가져온다.
// BlackboardPreconditionNode는 자식 노드 하나를 감싸서
// 실행 여부를 제어하는 데코레이터 노드이므로 이 헤더가 필요하다.
#include "../decorator_node.h"

namespace BT
{

/**
 * BlackboardPreconditionNode
 *
 * Blackboard 또는 입력 포트에서 받은 두 값을 비교해서,
 * 값이 같을 때만 자식 노드를 실행하는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 특정 조건이 만족될 때만 자식 노드를 실행하고 싶을 때 사용한다.
 * - 예를 들어 Blackboard에 저장된 값이 기대값과 같은지 확인한 뒤,
 *   조건이 맞으면 공격/회피/탐색 같은 자식 행동을 실행할 수 있다.
 *
 * 동작 방식:
 * - value_A와 value_B를 비교한다.
 * - 두 값이 같으면 자식 노드를 실행하고, 자식 노드의 결과를 그대로 반환한다.
 * - 두 값이 다르면 자식 노드를 실행하지 않고 return_on_mismatch 값을 반환한다.
 *
 * 사용 예시:
 *
 * <BlackboardCheckInt value_A="{the_answer}"
 *                     value_B="42"
 *                     return_on_mismatch="FAILURE" />
 *
 * 위 예시는 Blackboard의 the_answer 값이 42인지 확인한다.
 * 같으면 자식 노드를 실행하고, 다르면 FAILURE를 반환한다.
 */
template <typename T>
class BlackboardPreconditionNode : public DecoratorNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * config:
     * - XML 포트 정보와 Blackboard 연결 정보를 담고 있는 설정 객체이다.
     *
     * template <typename T>:
     * - 비교할 값의 자료형을 의미한다.
     * - int, double, std::string 같은 타입에 따라 등록 이름이 달라진다.
     */
    BlackboardPreconditionNode(const std::string& name, const NodeConfiguration& config)
      : DecoratorNode(name, config)
    {
        // T가 int 타입이면 XML에서 BlackboardCheckInt라는 이름으로 사용할 수 있게 한다.
        if( std::is_same<T,int>::value)
            setRegistrationID("BlackboardCheckInt");

        // T가 double 타입이면 XML에서 BlackboardCheckDouble이라는 이름으로 사용할 수 있게 한다.
        else if( std::is_same<T,double>::value)
            setRegistrationID("BlackboardCheckDouble");

        // T가 std::string 타입이면 XML에서 BlackboardCheckString이라는 이름으로 사용할 수 있게 한다.
        else if( std::is_same<T,std::string>::value)
            setRegistrationID("BlackboardCheckString");
    }

    /**
     * 소멸자
     *
     * 특별한 정리 작업이 없으므로 기본 소멸자를 사용한다.
     */
    virtual ~BlackboardPreconditionNode() override = default;

    /**
     * providedPorts()
     *
     * 이 노드가 XML에서 사용할 수 있는 입력 포트 목록을 정의한다.
     *
     * value_A:
     * - 비교할 첫 번째 값이다.
     * - 보통 Blackboard 값을 가져올 때 사용한다.
     *
     * value_B:
     * - 비교할 두 번째 값이다.
     * - 기대값 또는 기준값으로 사용한다.
     *
     * return_on_mismatch:
     * - value_A와 value_B가 다를 때 반환할 상태값이다.
     * - 예: FAILURE, SUCCESS, RUNNING
     */
    static PortsList providedPorts()
    {
        return {
            InputPort("value_A"),
            InputPort("value_B"),
            InputPort<NodeStatus>("return_on_mismatch")
        };
    }

  private:
    /**
     * tick()
     *
     * Behavior Tree가 이 노드를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. value_A 값을 읽는다.
     * 2. value_B 값을 읽는다.
     * 3. 두 값이 같은지 비교한다.
     * 4. 같으면 자식 노드를 실행한다.
     * 5. 다르면 자식 노드를 실행하지 않는다.
     * 6. 불일치 시 return_on_mismatch 값을 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

//----------------------------------------------------

/**
 * BlackboardPreconditionNode의 tick() 구현부
 *
 * template 클래스이므로 헤더 파일 안에 구현되어 있다.
 */
template<typename T> inline
NodeStatus BlackboardPreconditionNode<T>::tick()
{
    T value_A;     // 비교할 첫 번째 값
    T value_B;     // 비교할 두 번째 값

    // 조건이 맞지 않을 때 기본적으로 반환할 상태값이다.
    // XML에서 return_on_mismatch를 따로 지정하지 않으면 FAILURE를 반환한다.
    NodeStatus default_return_status = NodeStatus::FAILURE;

    // 현재 노드가 실행 중이라는 상태로 설정한다.
    setStatus(NodeStatus::RUNNING);

    // value_A와 value_B를 모두 읽는 데 성공했고,
    // 두 값이 같다면 자식 노드를 실행한다.
    if( getInput("value_A", value_A) &&
        getInput("value_B", value_B) &&
        value_B == value_A )
    {
        // 조건이 만족되었으므로 자식 노드를 실행하고,
        // 자식 노드의 실행 결과를 그대로 반환한다.
        return child_node_->executeTick();
    }

    // 조건이 맞지 않았는데 자식 노드가 아직 RUNNING 상태라면,
    // 더 이상 실행하면 안 되므로 자식 노드를 중지한다.
    if( child()->status() == NodeStatus::RUNNING )
    {
        haltChild();
    }

    // value_A와 value_B가 다를 때 반환할 상태값을 포트에서 읽는다.
    // 포트가 없으면 기본값인 FAILURE가 유지된다.
    getInput("return_on_mismatch", default_return_status);

    // 조건 불일치 시 지정된 상태값을 반환한다.
    return default_return_status;
}

} // namespace BT

#endif