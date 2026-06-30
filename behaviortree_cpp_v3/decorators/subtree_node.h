#ifndef DECORATOR_SUBTREE_NODE_H
#define DECORATOR_SUBTREE_NODE_H

// DecoratorNode 관련 정의를 가져온다.
// SubtreeNode와 SubtreeWrapperNode는 자식 노드 하나를 감싸서 실행하는 구조이므로
// DecoratorNode를 상속받기 위해 이 헤더가 필요하다.
#include "../decorator_node.h"

namespace BT
{

/**
 * SubtreeNode
 *
 * 하나의 Subtree 전체를 감싸서 실행하는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 큰 Behavior Tree 안에서 별도의 작은 Behavior Tree를 실행할 때 사용한다.
 * - 공격 행동, 회피 행동, 탐색 행동처럼 복잡한 행동 로직을
 *   하나의 Subtree로 분리해서 관리할 수 있다.
 *
 * 특징:
 * - SubtreeNode는 별도의 Blackboard를 생성한다.
 * - 부모 트리의 Blackboard와 Subtree의 Blackboard가 분리되어 있다.
 * - 따라서 부모 트리와 Subtree 사이에서 데이터를 주고받으려면
 *   포트 remapping을 명시적으로 해줘야 한다.
 *
 * 쉽게 말하면:
 * - SubtreeNode는 독립적인 작업 공간을 가진 Subtree이다.
 * - 부모 트리의 데이터를 자동으로 공유하지 않는다.
 */
class SubtreeNode : public DecoratorNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 SubtreeNode를 구분하기 위한 이름이다.
     *
     * SubtreeNode는 DecoratorNode를 상속받아
     * 하나의 Subtree를 감싸는 역할을 한다.
     */
    SubtreeNode(const std::string& name);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~SubtreeNode() override = default;

  private:
    /**
     * tick()
     *
     * Behavior Tree가 SubtreeNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작:
     * - 감싸고 있는 Subtree를 실행한다.
     * - Subtree의 실행 결과를 현재 노드의 결과로 반환한다.
     *
     * 반환:
     * - Subtree가 SUCCESS이면 SUCCESS 반환
     * - Subtree가 FAILURE이면 FAILURE 반환
     * - Subtree가 RUNNING이면 RUNNING 반환
     */
    virtual BT::NodeStatus tick() override;

    /**
     * type()
     *
     * 이 노드의 타입을 반환하는 함수이다.
     *
     * NodeType::SUBTREE:
     * - 이 노드가 일반 Action, Condition, Control, Decorator가 아니라
     *   Subtree를 나타내는 노드임을 의미한다.
     *
     * final:
     * - 이 함수가 하위 클래스에서 다시 재정의되지 못하게 막는다.
     */
    virtual NodeType type() const override final
    {
        return NodeType::SUBTREE;
    }
};

/**
 * SubtreeWrapperNode
 *
 * 하나의 Subtree 전체를 감싸서 실행하는 데코레이터 노드이다.
 *
 * 사용 목적:
 * - 큰 Behavior Tree 안에서 다른 Subtree를 실행할 때 사용한다.
 * - SubtreeNode와 비슷하지만 Blackboard 처리 방식이 다르다.
 *
 * 특징:
 * - 별도의 Blackboard를 생성하지 않는다.
 * - 부모 트리와 같은 Blackboard를 공유한다.
 * - 따라서 포트 remapping 없이도 부모 트리의 데이터를 그대로 사용할 수 있다.
 *
 * 쉽게 말하면:
 * - SubtreeWrapperNode는 부모 트리와 같은 작업 공간을 사용하는 Subtree이다.
 * - 부모 트리의 데이터를 Subtree 안에서도 바로 사용할 수 있다.
 *
 * 주의:
 * - 기존 주석에는 TransparentSubtreeNode라고 적혀 있지만,
 *   실제 클래스 이름은 SubtreeWrapperNode이다.
 */
class SubtreeWrapperNode : public DecoratorNode
{
  public:
    /**
     * 생성자
     *
     * name:
     * - Behavior Tree 안에서 이 SubtreeWrapperNode를 구분하기 위한 이름이다.
     *
     * SubtreeWrapperNode는 DecoratorNode를 상속받아
     * 하나의 Subtree를 감싸는 역할을 한다.
     */
    SubtreeWrapperNode(const std::string& name);

    /**
     * 소멸자
     *
     * override:
     * - 부모 클래스의 virtual 소멸자를 재정의한다는 의미이다.
     *
     * default:
     * - 특별한 정리 작업 없이 컴파일러가 기본 소멸자를 사용하도록 한다.
     */
    virtual ~SubtreeWrapperNode() override = default;

  private:
    /**
     * tick()
     *
     * Behavior Tree가 SubtreeWrapperNode를 실행할 때 호출되는 핵심 함수이다.
     *
     * 동작:
     * - 감싸고 있는 Subtree를 실행한다.
     * - Subtree의 실행 결과를 현재 노드의 결과로 반환한다.
     * - 부모 트리와 같은 Blackboard를 공유하므로
     *   별도의 포트 remapping 없이 데이터 접근이 가능하다.
     */
    virtual BT::NodeStatus tick() override;

    /**
     * type()
     *
     * 이 노드의 타입을 반환하는 함수이다.
     *
     * NodeType::SUBTREE:
     * - 이 노드가 Subtree를 나타내는 노드임을 의미한다.
     *
     * final:
     * - 이 함수가 하위 클래스에서 다시 재정의되지 못하게 막는다.
     */
    virtual NodeType type() const override final
    {
        return NodeType::SUBTREE;
    }
};

} // namespace BT

#endif   // DECORATOR_SUBTREE_NODE_H