using System;
using System.Collections.Generic;

namespace KeepCryingEngine.BehaviourTree
{
    class BehaviourTreeBuilder
    {
        Stack<BehaviourTreeNode> m_nodeStack;

        public BehaviourTreeBuilder Action(string name, Func<DataContext, BehaviourTreeStatus> func)
        {
            ActionNode actionNode = new ActionNode(name, func);
            AddAsChildToTopStackNodeOrFail(actionNode);
            return this;
        }

        public BehaviourTreeBuilder Condition(string name, Func<DataContext, bool> func)
        {
            return Action(name, (t) => func(t) ? BehaviourTreeStatus.Success : BehaviourTreeStatus.Error);
        }

        public BehaviourTreeBuilder Decorator(string name, DecoratorNode decoratorNode)
        {
            AddAsChildToTopStackNodeIfPossibleAndPushToStack(decoratorNode);
            return this;
        }

        public BehaviourTreeBuilder Composite(string name, CompositeNode compositeNode)
        {
            AddAsChildToTopStackNodeIfPossibleAndPushToStack(compositeNode);
            return this;
        }

        public BehaviourTreeBuilder End()
        {
            if (m_nodeStack.Count > 1)
            {
                throw new ApplicationException("Canot end the construction on this state");
            }
            m_nodeStack.Pop();
            return this;
        }

        public BehaviourTreeNode Build()
        {
            if(m_nodeStack.Count != 1)
            {
                throw new ApplicationException("Can't build the behaviour tree");
            }

            return m_nodeStack.Pop();
        }

        private void AddAsChildToTopStackNodeIfPossibleAndPushToStack(BehaviourTreeNode node)
        {
            if (m_nodeStack.Count > 0)
            {
                m_nodeStack.Peek().AddChild(node);
            }
            m_nodeStack.Push(node);
        }

        private void AddAsChildToTopStackNodeOrFail(BehaviourTreeNode child)
        {
            if (m_nodeStack.Count > 0)
            {
                m_nodeStack.Peek().AddChild(child);
            }
            else
            {
                throw new ApplicationException("Can't add \"" + child.Name + "\" because it needs to have a parent");
            }
        }
    }
}
