using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KeepCryingEngine.BehaviourTree
{
    public abstract class BehaviourTreeNode
    {
        public string Name { get; private set; }

        protected BehaviourTreeNode(string name)
        {
            Name = name;
        }

        public abstract BehaviourTreeStatus Tick(DataContext dataContext);

        public abstract void AddChild(BehaviourTreeNode child);
    }
}
