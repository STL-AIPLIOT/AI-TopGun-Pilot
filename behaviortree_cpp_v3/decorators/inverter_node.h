/* Copyright (C) 2018 Michele Colledanchise -  All Rights Reserved
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

#ifndef DECORATOR_INVERTER_NODE_H
#define DECORATOR_INVERTER_NODE_H

// DecoratorNode 관련 정의를 가져온다.
// InverterNode는 자식 노드 하나를 감싸서 결과를 반대로 바꾸는 데코레이터 노드이므로
// DecoratorNode를 상속받기 위해 이 헤더가 필요하다.
#include "../decorator_node.h"

namespace BT
{

/**
 * InverterNode
 *
 * 자식 노드의 실행 결과를 반대로 바꾸는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 어떤 조건의 결과를 반대로 해석하고 싶을 때 사용한다.
 * - 예를 들어 "적이 보인다" 조건을 반대로 바꿔서
 *   "적이 보이지 않는다"라는 조건처럼 사용할 수 있다.
 *
 * 동작 방식:
 * - 자식 노드가 SUCCESS를 반환하면 InverterNode는 FAILURE를 반환한다.
 * - 자식 노드가 FAILURE를 반환하면 InverterNode는 SUCCESS를 반환한다.
 * - 자식 노드가 RUNNING을 반환하면 아직 실행 중이므로 RUNNING을 그대로 반환한다.
 */
class InverterNode : public DecoratorNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 노드를 구분하기 위한 이름이다.
     *
     * InverterNode는 DecoratorNode를 상속받아
     * 자식 노드 하나의 반환 결과를 반대로 바꾸는 역할을 한다.
     */
    InverterNode(const std::string& name);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~InverterNode() override = default;

  private:
    /**
     * tick()
     *
     * Behavior Tree가 InverterNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작 순서:
     * 1. 자식 노드를 실행한다.
     * 2. 자식이 SUCCESS이면 FAILURE를 반환한다.
     * 3. 자식이 FAILURE이면 SUCCESS를 반환한다.
     * 4. 자식이 RUNNING이면 RUNNING을 그대로 반환한다.
     */
    virtual BT::NodeStatus tick() override;
};

} // namespace BT

#endif
