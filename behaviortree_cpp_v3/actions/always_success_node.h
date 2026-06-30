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

#ifndef ACTION_ALWAYS_SUCCESS_NODE_H
#define ACTION_ALWAYS_SUCCESS_NODE_H

// 부모 클래스인 ActionNode 관련 정의를 가져온다.
// AlwaysSuccessNode는 SyncActionNode를 상속받기 때문에 이 헤더가 필요하다.
#include "../action_node.h"

namespace BT
{

/**
 * AlwaysSuccessNode
 *
 * 항상 SUCCESS 상태를 반환하는 동기 액션 노드이다.
 *
 * 사용 목적:
 * - Behavior Tree에서 특정 행동을 무조건 성공 처리하고 싶을 때 사용한다.
 * - Sequence 노드에서 다음 단계로 흐름을 넘기고 싶을 때 유용하다.
 * - 실제 행동을 수행하지 않고 성공 결과만 반환하는 테스트용/제어용 노드이다.
 */
class AlwaysSuccessNode : public SyncActionNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * SyncActionNode(name, {}):
     * - 부모 클래스인 SyncActionNode의 생성자를 호출한다.
     * - 두 번째 인자인 {}는 이 노드가 별도의 입력/출력 포트를 사용하지 않는다는 뜻이다.
     */
    AlwaysSuccessNode(const std::string& name) :
        SyncActionNode(name, {})
    {
        // XML이나 Factory 등록 시 사용할 노드 이름을 지정한다.
        // Behavior Tree에서 "AlwaysSuccess"라는 이름으로 이 노드를 사용할 수 있게 한다.
        setRegistrationID("AlwaysSuccess");
    }

  private:
    /**
     * tick()
     *
     * Behavior Tree가 이 노드를 실행할 때 호출되는 핵심 함수이다.
     *
     * 이 노드는 어떤 조건이나 행동을 검사하지 않고,
     * 항상 NodeStatus::SUCCESS를 반환한다.
     */
    virtual BT::NodeStatus tick() override
    {
        return NodeStatus::SUCCESS;
    }
};

} // namespace BT

#endif