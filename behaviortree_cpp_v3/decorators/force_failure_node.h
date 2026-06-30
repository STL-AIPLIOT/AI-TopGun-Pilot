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

#ifndef DECORATOR_ALWAYS_FAILURE_NODE_H
#define DECORATOR_ALWAYS_FAILURE_NODE_H

// DecoratorNode 관련 정의를 가져온다.
// ForceFailureNode는 자식 노드 하나를 감싸서 결과를 바꾸는 데코레이터 노드이므로
// DecoratorNode를 상속받기 위해 이 헤더가 필요하다.
#include "../decorator_node.h"

namespace BT
{

/**
 * ForceFailureNode
 *
 * 자식 노드를 실행한 뒤,
 * 자식 노드가 SUCCESS 또는 FAILURE를 반환하면 무조건 FAILURE로 바꾸는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 자식 노드의 성공 여부와 관계없이 결과를 실패로 처리하고 싶을 때 사용한다.
 * - 특정 행동은 실행하되, Behavior Tree 흐름상 실패한 것처럼 만들고 싶을 때 사용한다.
 *
 * 동작 방식:
 * - 자식 노드가 SUCCESS를 반환하면 ForceFailureNode는 FAILURE를 반환한다.
 * - 자식 노드가 FAILURE를 반환해도 ForceFailureNode는 FAILURE를 반환한다.
 * - 자식 노드가 RUNNING을 반환하면 아직 실행 중이므로 RUNNING을 그대로 반환한다.
 */
class ForceFailureNode : public DecoratorNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * DecoratorNode(name, {}):
     * - 부모 클래스인 DecoratorNode의 생성자를 호출한다.
     * - 두 번째 인자인 {}는 별도의 포트 설정을 사용하지 않는다는 뜻이다.
     */
    ForceFailureNode(const std::string& name) :
        DecoratorNode(name, {})
    {
        // XML이나 Factory 등록 시 사용할 노드 이름을 지정한다.
        // Behavior Tree에서 "ForceFailure"라는 이름으로 이 노드를 사용할 수 있게 한다.
        setRegistrationID("ForceFailure");
    }

  private:
    /**
     * tick()
     *
     * Behavior Tree가 ForceFailureNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 현재 노드 상태를 RUNNING으로 설정한다.
     * 2. 자식 노드를 실행한다.
     * 3. 자식이 SUCCESS 또는 FAILURE이면 FAILURE를 반환한다.
     * 4. 자식이 RUNNING이면 RUNNING을 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

//------------ implementation ----------------------------

/**
 * ForceFailureNode의 tick() 구현부
 *
 * 자식 노드를 실행한 뒤,
 * 자식 노드의 결과를 ForceFailureNode 규칙에 맞게 변환한다.
 */
inline NodeStatus ForceFailureNode::tick()
{
    // 현재 데코레이터 노드가 실행 중이라는 상태로 설정한다.
    setStatus(NodeStatus::RUNNING);

    // 감싸고 있는 자식 노드를 실행하고 그 결과를 child_state에 저장한다.
    const NodeStatus child_state = child_node_->executeTick();

    // 자식 노드의 실행 결과에 따라 반환할 상태를 결정한다.
    switch (child_state)
    {
        case NodeStatus::FAILURE:
        case NodeStatus::SUCCESS:
        {
            // 자식이 실패했든 성공했든 최종 결과는 무조건 FAILURE로 바꾼다.
            return NodeStatus::FAILURE;
        }

        case NodeStatus::RUNNING:
        {
            // 자식이 아직 실행 중이면 강제로 실패 처리하지 않고 RUNNING을 유지한다.
            return NodeStatus::RUNNING;
        }

        default:
        {
            // 정의되지 않은 상태가 들어온 경우를 대비한 영역이다.
            // 현재 코드에서는 별도 예외 처리가 구현되어 있지 않다.
            // TODO throw?
        }
    }

    // 예외적인 경우 현재 노드의 상태를 그대로 반환한다.
    return status();
}

} // namespace BT

#endif
