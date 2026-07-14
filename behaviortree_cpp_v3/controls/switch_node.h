/* Copyright (C) 2020-2020 Davide Faconti -  All Rights Reserved
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
#ifndef SWITCH_NODE_H
#define SWITCH_NODE_H

// SwitchNode는 여러 자식 노드 중 하나를 선택해서 실행하는 제어 노드이므로
// ControlNode를 상속받기 위해 이 헤더가 필요하다.
#include "../control_node.h"

namespace BT
{

/**
 * SwitchNode
 *
 * 입력된 variable 값과 case 값들을 비교해서,
 * 조건에 맞는 자식 노드 하나를 선택해 실행하는 제어 노드이다.
 *
 * 사용 목적:
 * - 상황 값에 따라 실행할 행동을 다르게 선택할 때 사용한다.
 * - C/C++의 switch-case 문처럼 동작한다.
 *
 * 구조:
 * - variable 값과 case_1, case_2, ... 값을 비교한다.
 * - 일치하는 case가 있으면 해당 번호의 자식 노드를 실행한다.
 * - 일치하는 case가 없으면 마지막 default 자식 노드를 실행한다.
 *
 * 예:
 * - variable = "attack"
 * - case_1 = "attack"
 * - case_2 = "escape"
 *
 * 이 경우 case_1과 일치하므로 첫 번째 자식 노드를 실행한다.
 */
template <size_t NUM_CASES>
class SwitchNode : public ControlNode
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
     * running_child_(-1):
     * - 현재 RUNNING 중인 자식 노드가 없다는 의미이다.
     */
    SwitchNode(const std::string& name, const BT::NodeConfiguration& config)
    : ControlNode::ControlNode(name, config ),
      running_child_(-1)
    {
        // XML이나 Factory 등록 시 사용할 노드 이름을 지정한다.
        // Behavior Tree에서 "Switch"라는 이름으로 이 노드를 사용할 수 있게 한다.
        setRegistrationID("Switch");
    }

    /**
     * 소멸자
     *
     * 특별한 정리 작업이 없으므로 기본 소멸자를 사용한다.
     */
    virtual ~SwitchNode() override = default;

    /**
     * halt()
     *
     * SwitchNode 실행을 중지할 때 호출된다.
     *
     * 동작:
     * - 현재 실행 중인 자식 정보를 초기화한다.
     * - 부모 클래스의 halt()를 호출해서 자식 노드들도 중지시킨다.
     */
    void halt() override
    {
        // 실행 중인 자식 노드가 없다는 상태로 초기화한다.
        running_child_ = -1;

        // ControlNode의 halt()를 호출하여 자식 노드들을 중지한다.
        ControlNode::halt();
    }

    /**
     * providedPorts()
     *
     * SwitchNode가 XML에서 사용할 수 있는 포트 목록을 정의한다.
     *
     * variable:
     * - 비교 기준이 되는 값이다.
     *
     * case_1, case_2, ...:
     * - variable과 비교할 case 값들이다.
     *
     * NUM_CASES 값에 따라 case 포트 개수가 결정된다.
     */
    static PortsList providedPorts()
    {
        PortsList ports;

        // 비교 기준이 되는 variable 입력 포트를 추가한다.
        ports.insert( BT::InputPort<std::string>("variable") );

        // NUM_CASES 개수만큼 case_1, case_2, ... 포트를 자동으로 생성한다.
        for(unsigned i=0; i < NUM_CASES; i++)
        {
            char case_str[20];

            // i가 0이면 case_1, i가 1이면 case_2 형식으로 포트 이름을 만든다.
            sprintf(case_str, "case_%d", i+1);

            // 생성한 case 포트를 입력 포트로 등록한다.
            ports.insert( BT::InputPort<std::string>(case_str) );
        }

        return ports;
    }

  private:
    /**
     * running_child_
     *
     * 현재 RUNNING 상태인 자식 노드의 인덱스를 저장한다.
     *
     * -1:
     * - 현재 실행 중인 자식 노드가 없다는 의미이다.
     *
     * 필요한 이유:
     * - 이전 tick에서 실행 중이던 자식과
     *   이번 tick에서 선택된 자식이 다르면 이전 자식을 halt하기 위해 사용한다.
     */
    int running_child_;

    /**
     * tick()
     *
     * Behavior Tree가 SwitchNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 자식 노드 개수가 올바른지 확인한다.
     * 2. variable 값을 읽는다.
     * 3. variable과 case_1, case_2, ... 값을 비교한다.
     * 4. 일치하는 case가 있으면 해당 자식 노드를 선택한다.
     * 5. 일치하는 case가 없으면 default 자식 노드를 선택한다.
     * 6. 선택된 자식 노드를 실행한다.
     * 7. 결과가 RUNNING이면 현재 자식 인덱스를 저장한다.
     * 8. SUCCESS 또는 FAILURE이면 자식들을 정리하고 결과를 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

/**
 * SwitchNode의 tick() 구현부
 *
 * template 클래스이므로 헤더 파일 안에 구현되어 있다.
 */
template<size_t NUM_CASES> inline
NodeStatus SwitchNode<NUM_CASES>::tick()
{
    // 자주 사용하는 case_1 ~ case_9 포트 이름을 미리 저장해 둔다.
    // 9개 이하의 case는 이 배열을 사용하고,
    // 10개 이상부터는 아래에서 문자열을 직접 만들어 사용한다.
    constexpr const char * case_port_names[9] = {
      "case_1", "case_2", "case_3", "case_4", "case_5", "case_6", "case_7", "case_8", "case_9"};

    // SwitchNode의 자식 개수는 반드시 NUM_CASES + 1이어야 한다.
    // +1은 어떤 case와도 일치하지 않을 때 실행할 default 자식 노드이다.
    if( childrenCount() != NUM_CASES+1)
    {
        throw LogicError("Wrong number of children in SwitchNode; "
                         "must be (num_cases + default)");
    }

    std::string variable;   // 비교 기준 값
    std::string value;      // 각 case 포트에서 읽어올 값

    // 기본값은 default 자식 노드의 인덱스이다.
    // case가 하나도 일치하지 않으면 마지막 자식 노드가 실행된다.
    int child_index = NUM_CASES; // default index;

    // variable 포트 값을 읽는다.
    // variable 값이 없으면 case 비교를 하지 않고 default 자식으로 이동한다.
    if (getInput("variable", variable))
    {
        // case_1부터 case_N까지 순서대로 확인한다.
        for (unsigned index = 0; index < NUM_CASES; ++index)
        {
            bool found = false;

            // case_1 ~ case_9까지는 미리 만들어 둔 배열에서 포트 이름을 가져온다.
            if( index < 9 )
            {
                found = (bool)getInput(case_port_names[index], value);
            }
            else
            {
                // case_10 이상부터는 직접 포트 이름을 만들어 사용한다.
                char case_str[20];
                sprintf(case_str, "case_%d", index+1);
                found = (bool)getInput(case_str, value);
            }

            // case 포트가 존재하고, variable 값과 case 값이 같으면
            // 해당 case 번호의 자식 노드를 선택한다.
            if (found && variable == value)
            {
                child_index = index;
                break;
            }
        }
    }

    // 이전 tick에서 실행 중이던 자식이 있고,
    // 이번에 선택된 자식과 다르다면 이전 자식을 중지한다.
    if( running_child_ != -1 && running_child_ != child_index)
    {
        haltChild(running_child_);
    }

    // 선택된 자식 노드를 가져온다.
    auto& selected_child = children_nodes_[child_index];

    // 선택된 자식 노드를 실행한다.
    NodeStatus ret = selected_child->executeTick();

    // 선택된 자식이 아직 실행 중이면,
    // 다음 tick에서 이어서 관리할 수 있도록 인덱스를 저장한다.
    if( ret == NodeStatus::RUNNING )
    {
        running_child_ = child_index;
    }
    else
    {
        // SUCCESS 또는 FAILURE가 나오면 실행이 끝난 것이므로
        // 모든 자식을 정리하고 running_child_를 초기화한다.
        haltChildren();
        running_child_ = -1;
    }

    // 선택된 자식 노드의 실행 결과를 그대로 반환한다.
    return ret;
}

}

#endif // SWITCH_NODE_H
