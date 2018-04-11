using System;
using System.Collections.Generic;

namespace KeepCryingEngine.BehaviourTree
{
    class BehaviourTreeBuilder
    {
        BehaviourTreeNode m_currentNode;

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

        public BehaviourTreeBuilder Action(string name)
        {
            InverterDecoratorNode inverterDecoratorNode = new InverterDecoratorNode(name);
            AddAsChildToTopStackNodeIfPossibleAndPushToStack(inverterDecoratorNode);

            return this;
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
