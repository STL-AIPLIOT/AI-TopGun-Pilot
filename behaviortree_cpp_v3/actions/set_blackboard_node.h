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

#ifndef ACTION_SETBLACKBOARD_NODE_H
#define ACTION_SETBLACKBOARD_NODE_H

// 부모 클래스인 ActionNode 관련 정의를 가져온다.
// SetBlackboard는 SyncActionNode를 상속받기 때문에 이 헤더가 필요하다.
#include "../action_node.h"

namespace BT
{

/**
 * SetBlackboard
 *
 * Blackboard에 문자열 값을 저장하는 동기 액션 노드이다.
 *
 * 사용 목적:
 * - Behavior Tree 실행 중 특정 값을 Blackboard에 저장할 때 사용한다.
 * - 다른 노드들이 같은 Blackboard 값을 읽어서 조건 판단이나 행동에 활용할 수 있다.
 *
 * 사용 예시:
 *
 * <SetBlackboard value="42" output_key="the_answer" />
 *
 * 위 예시는 문자열 "42"를 Blackboard의 "the_answer"라는 key에 저장한다.
 */
class SetBlackboard : public SyncActionNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * config:
     * - 이 노드가 사용할 포트 정보와 Blackboard 연결 정보를 담고 있다.
     *
     * SyncActionNode(name, config):
     * - 부모 클래스인 SyncActionNode의 생성자를 호출한다.
     */
    SetBlackboard(const std::string& name, const NodeConfiguration& config)
      : SyncActionNode(name, config)
    {
        // XML이나 Factory 등록 시 사용할 노드 이름을 지정한다.
        // Behavior Tree에서 "SetBlackboard"라는 이름으로 이 노드를 사용할 수 있게 한다.
        setRegistrationID("SetBlackboard");
    }

    /**
     * providedPorts()
     *
     * 이 노드가 XML에서 사용할 수 있는 포트 목록을 정의한다.
     *
     * value:
     * - Blackboard에 저장할 값이다.
     * - 문자열 형태로 전달된다.
     *
     * output_key:
     * - 값을 저장할 Blackboard의 key 이름이다.
     * - BidirectionalPort이므로 입력처럼 읽을 수도 있고 출력처럼 값을 쓸 수도 있다.
     */
    static PortsList providedPorts()
    {
        return {
            InputPort(
                "value",
                "Value represented as a string. convertFromString must be implemented."
            ),

            BidirectionalPort(
                "output_key",
                "Name of the blackboard entry where the value should be written"
            )
        };
    }

  private:
    /**
     * tick()
     *
     * Behavior Tree가 이 노드를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. output_key 포트에서 Blackboard key 이름을 읽는다.
     * 2. value 포트에서 저장할 값을 읽는다.
     * 3. output_key에 value를 저장한다.
     * 4. 저장이 성공하면 SUCCESS를 반환한다.
     */
    virtual BT::NodeStatus tick() override
    {
        std::string key, value;

        // output_key 포트가 없으면 값을 어디에 저장할지 알 수 없으므로 에러를 발생시킨다.
        if ( !getInput("output_key", key) )
        {
            throw RuntimeError("missing port [output_key]");
        }

        // value 포트가 없으면 저장할 값이 없으므로 에러를 발생시킨다.
        if ( !getInput("value", value) )
        {
            throw RuntimeError("missing port [value]");
        }

        // output_key 포트가 가리키는 Blackboard 위치에 value 값을 저장한다.
        setOutput("output_key", value);

        // Blackboard 저장이 완료되었으므로 SUCCESS를 반환한다.
        return NodeStatus::SUCCESS;
    }
};

} // namespace BT

#endif