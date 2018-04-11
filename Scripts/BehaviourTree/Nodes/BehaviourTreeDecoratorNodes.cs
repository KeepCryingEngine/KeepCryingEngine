using System;
using System.Collections.Generic;

namespace KeepCryingEngine.BehaviourTree
{
    public abstract class DecoratorNode : BehaviourTreeNode
    {
        public BehaviourTreeNode Child { get; private set; }

        public DecoratorNode(string name) : base(name) { }

        public override void AddChild(BehaviourTreeNode child)
        {
            if(Child != null)
            {
                Child = child;
            }
            else
            {
                throw new ApplicationException("Can't add more than a single child to " + Name);
            }
        }
    }

    public class InverterDecoratorNode : DecoratorNode
    {
        public InverterDecoratorNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            BehaviourTreeStatus status = Child.Tick(dataContext);
            return Invert(status);
        }

        private static BehaviourTreeStatus Invert(BehaviourTreeStatus status)
        {
            switch (status)
            {
                case BehaviourTreeStatus.Running:
                    return BehaviourTreeStatus.Running;
                case BehaviourTreeStatus.Success:
                    return BehaviourTreeStatus.Failure;
                case BehaviourTreeStatus.Failure:
                    return BehaviourTreeStatus.Success;
                default:
                    return BehaviourTreeStatus.Error;
            }
        }
    }

    public class SuccederDecoratorNode : DecoratorNode
    {
        public SuccederDecoratorNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            Child.Tick(dataContext);
            return BehaviourTreeStatus.Success;
        }
    }

    public class FailerDecoratorNode : DecoratorNode
    {
        public FailerDecoratorNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            Child.Tick(dataContext);
            return BehaviourTreeStatus.Failure;
        }
    }

    public class LimiterDecoratorNode : DecoratorNode
    {
        private class Variables
        {
            public static string currentTicks = "currentTicks";
        }

        public int MaxTicks { get; set; }

        public LimiterDecoratorNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            dataContext.Get(Name + Variables.currentTicks, out int tickCount);
            if (tickCount < MaxTicks)
            {
                tickCount++;
                dataContext.Set(Name + Variables.currentTicks, tickCount);

                return Child.Tick(dataContext);
            }
            return BehaviourTreeStatus.Failure;               
        }
    }
}
