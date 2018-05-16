using System;

namespace KeepCryingEngine.BehaviourTree
{
    public abstract class LeafNode : BehaviourTreeNode
    {
        protected LeafNode(string name) : base(name)
        {
        }

        public override void AddChild(BehaviourTreeNode child)
        {
            throw new ApplicationException("Can't add child to " + Name);
        }
    }

    public class ActionNode : LeafNode
    {
        private Func<DataContext, BehaviourTreeStatus> m_func;

        public ActionNode(string name, Func<DataContext, BehaviourTreeStatus> func) : base(name)
        {
            m_func = func;
        }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            return m_func.Invoke(dataContext);
        }
    }
}
