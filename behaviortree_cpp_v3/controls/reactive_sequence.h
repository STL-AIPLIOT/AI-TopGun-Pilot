/* Copyright (C) 2020 Davide Faconti, Eurecat -  All Rights Reserved
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

#ifndef REACTIVE_SEQUENCE_NODE_H
#define REACTIVE_SEQUENCE_NODE_H

// ControlNode 관련 정의를 가져온다.
// ReactiveSequence는 여러 자식 노드를 순서대로 실행하는 제어 노드이므로
// ControlNode를 상속받기 위해 이 헤더가 필요하다.
#include "../control_node.h"

namespace BT
{

/**
 * ReactiveSequence
 *
 * 여러 자식 노드를 첫 번째부터 마지막까지 순서대로 실행하면서,
 * 하나라도 FAILURE가 나오면 즉시 FAILURE를 반환하는 제어 노드이다.
 *
 * 사용 목적:
 * - 여러 조건이나 행동을 순서대로 검사할 때 사용한다.
 * - 모든 자식 노드가 성공해야 전체 성공으로 판단한다.
 * - 매 tick마다 앞쪽 자식부터 다시 확인하기 때문에 상황 변화에 빠르게 반응할 수 있다.
 *
 * 동작 방식:
 * - 자식 노드가 SUCCESS를 반환하면 다음 자식 노드를 실행한다.
 * - 자식 노드가 RUNNING을 반환해도 다음 자식 노드를 계속 실행한다.
 * - 자식 노드가 FAILURE를 반환하면 즉시 실행을 멈추고 FAILURE를 반환한다.
 * - 모든 자식 노드가 SUCCESS를 반환하면 ReactiveSequence도 SUCCESS를 반환한다.
 *
 * 주의 사항:
 * - 비동기 자식 노드는 하나 이하로 두는 것이 좋다.
 * - 여러 비동기 자식이 동시에 RUNNING 상태가 되면 실행 흐름이 복잡해질 수 있다.
 */
class ReactiveSequence : public ControlNode
{
  public:

    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * ControlNode(name, {}):
     * - 부모 클래스인 ControlNode의 생성자를 호출한다.
     * - 두 번째 인자인 {}는 별도의 포트 설정을 사용하지 않는다는 뜻이다.
     */
    ReactiveSequence(const std::string& name):
        ControlNode(name, {}) {}

  private:

    /**
     * tick()
     *
     * Behavior Tree가 ReactiveSequence를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 첫 번째 자식 노드부터 순서대로 실행한다.
     * 2. 자식이 SUCCESS를 반환하면 다음 자식으로 넘어간다.
     * 3. 자식이 RUNNING을 반환해도 다음 자식을 계속 실행한다.
     * 4. 자식이 FAILURE를 반환하면 즉시 FAILURE를 반환한다.
     * 5. 모든 자식이 SUCCESS이면 최종적으로 SUCCESS를 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

} // namespace BT

#endif  // REACTIVE_SEQUENCE_NODE_H
