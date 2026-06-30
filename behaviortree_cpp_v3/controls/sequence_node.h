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

#ifndef SEQUENCENODE_H
#define SEQUENCENODE_H

// ControlNode 관련 정의를 가져온다.
// SequenceNode는 여러 자식 노드를 순서대로 실행하는 제어 노드이므로
// ControlNode를 상속받기 위해 이 헤더가 필요하다.
#include "../control_node.h"

namespace BT
{

/**
 * SequenceNode
 *
 * 여러 자식 노드를 정해진 순서대로 실행하는 제어 노드이다.
 *
 * 사용 목적:
 * - 여러 조건이나 행동을 순서대로 수행해야 할 때 사용한다.
 * - 앞 단계가 성공해야 다음 단계로 넘어갈 수 있는 구조에 적합하다.
 *
 * 동작 방식:
 * - 모든 자식 노드가 SUCCESS를 반환하면 SequenceNode도 SUCCESS를 반환한다.
 * - 자식 노드 중 하나가 RUNNING을 반환하면 SequenceNode도 RUNNING을 반환한다.
 * - 자식 노드 중 하나가 FAILURE를 반환하면 즉시 FAILURE를 반환한다.
 *
 * 특징:
 * - RUNNING 상태가 발생하면 현재 실행 중인 자식 인덱스를 기억한다.
 * - 다음 tick에서 이전에 성공했던 자식들은 다시 실행하지 않고,
 *   RUNNING이 발생했던 자식부터 이어서 실행한다.
 */
class SequenceNode : public ControlNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * SequenceNode는 ControlNode를 상속받아
     * 여러 자식 노드를 순서대로 관리하는 역할을 한다.
     */
    SequenceNode(const std::string& name);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~SequenceNode() override = default;

    /**
     * halt()
     *
     * 실행 중인 SequenceNode를 중지할 때 호출된다.
     *
     * 사용 목적:
     * - RUNNING 상태였던 자식 노드를 멈춘다.
     * - current_child_idx_를 초기화해서 다음 실행 때 첫 번째 자식부터 다시 검사하게 한다.
     */
    virtual void halt() override;

  private:
    /**
     * current_child_idx_
     *
     * 현재 실행 중인 자식 노드의 인덱스를 저장하는 변수이다.
     *
     * 필요한 이유:
     * - 어떤 자식 노드가 RUNNING 상태였는지 기억하기 위해 사용한다.
     * - 다음 tick에서 처음부터 다시 실행하지 않고,
     *   이전에 실행 중이던 자식부터 이어서 실행할 수 있다.
     */
    size_t current_child_idx_;

    /**
     * tick()
     *
     * Behavior Tree가 SequenceNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 현재 자식 노드부터 실행한다.
     * 2. 자식이 SUCCESS를 반환하면 다음 자식으로 넘어간다.
     * 3. 자식이 RUNNING을 반환하면 현재 인덱스를 저장하고 RUNNING을 반환한다.
     * 4. 자식이 FAILURE를 반환하면 즉시 FAILURE를 반환한다.
     * 5. 모든 자식이 SUCCESS이면 최종적으로 SUCCESS를 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

} // namespace BT

#endif // SEQUENCENODE_H
