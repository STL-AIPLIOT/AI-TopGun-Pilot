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

#ifndef SEQUENCE_NODE_WITH_MEMORY_H
#define SEQUENCE_NODE_WITH_MEMORY_H

// ControlNode 관련 정의를 가져온다.
// SequenceStarNode는 여러 자식 노드를 순서대로 실행하는 제어 노드이므로
// ControlNode를 상속받기 위해 이 헤더가 필요하다.
#include "../control_node.h"

namespace BT
{

/**
 * SequenceStarNode
 *
 * 여러 자식 노드를 순서대로 실행하는 제어 노드이다.
 * 기본 SequenceNode와 비슷하지만, 현재 실행 위치를 기억하는 특징이 있다.
 *
 * 사용 목적:
 * - 여러 행동을 순서대로 실행해야 할 때 사용한다.
 * - 이전에 성공한 자식 노드를 매번 다시 실행하지 않고 싶을 때 사용한다.
 * - 시간이 오래 걸리는 행동 흐름을 중간부터 이어서 실행할 때 유용하다.
 *
 * 동작 방식:
 * - 모든 자식 노드가 SUCCESS를 반환하면 SequenceStarNode도 SUCCESS를 반환한다.
 * - 자식 노드가 RUNNING을 반환하면 현재 위치를 기억하고 RUNNING을 반환한다.
 * - 자식 노드가 FAILURE를 반환하면 현재 위치를 기억하고 FAILURE를 반환한다.
 *
 * 특징:
 * - current_child_idx_를 사용해서 현재 실행 중인 자식 노드 위치를 저장한다.
 * - 다음 tick에서 처음 자식부터 다시 시작하지 않고,
 *   이전에 멈춘 자식부터 이어서 실행한다.
 */
class SequenceStarNode : public ControlNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * SequenceStarNode는 ControlNode를 상속받아
     * 여러 자식 노드를 순서대로 관리하는 역할을 한다.
     */
    SequenceStarNode(const std::string& name);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~SequenceStarNode() override = default;

    /**
     * halt()
     *
     * 실행 중인 SequenceStarNode를 중지할 때 호출된다.
     *
     * 사용 목적:
     * - 현재 실행 중인 자식 노드를 멈춘다.
     * - current_child_idx_를 초기화해서 다음 실행 때 다시 처음부터 시작할 수 있게 한다.
     */
    virtual void halt() override;

  private:

    /**
     * current_child_idx_
     *
     * 현재 실행 중인 자식 노드의 인덱스를 저장하는 변수이다.
     *
     * 필요한 이유:
     * - 어떤 자식 노드까지 실행했는지 기억하기 위해 사용한다.
     * - 이전에 SUCCESS를 반환한 자식 노드를 다시 실행하지 않고,
     *   멈췄던 위치부터 이어서 실행할 수 있다.
     */
    size_t current_child_idx_;

    /**
     * tick()
     *
     * Behavior Tree가 SequenceStarNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. current_child_idx_가 가리키는 자식 노드부터 실행한다.
     * 2. 자식이 SUCCESS를 반환하면 다음 자식으로 넘어간다.
     * 3. 자식이 RUNNING을 반환하면 현재 위치를 기억하고 RUNNING을 반환한다.
     * 4. 자식이 FAILURE를 반환하면 현재 위치를 기억하고 FAILURE를 반환한다.
     * 5. 모든 자식이 SUCCESS이면 최종적으로 SUCCESS를 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

} // namespace BT

#endif   // SEQUENCE_NODE_WITH_MEMORY_H