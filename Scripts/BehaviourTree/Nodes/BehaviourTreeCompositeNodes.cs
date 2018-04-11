using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KeepCryingEngine.BehaviourTree
{
    public abstract class CompositeNode : BehaviourTreeNode
    {
        public List<BehaviourTreeNode> Childs { get; set; } = new List<BehaviourTreeNode>();

        public CompositeNode(string name) : base(name)
        {
        }

        public override void AddChild(BehaviourTreeNode child)
        {
            Childs.Add(child);
        }
    }

    public class SequenceCompositeNode : CompositeNode
    {
        private class Variables
        {
            public static string previousChild = "previousChild";
        }

        public SequenceCompositeNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            dataContext.Get<int>(Name + Variables.previousChild, out int currentChild);

            BehaviourTreeStatus status;
            do
            {
                status = Childs[currentChild].Tick(dataContext);
                if (status == BehaviourTreeStatus.Success)
                {
                    currentChild++;
                }
            } while (status == BehaviourTreeStatus.Success && currentChild < Childs.Count);

            if (currentChild < Childs.Count)
            {
                dataContext.Set(Name + Variables.previousChild, currentChild);
            }
            else
            {
                dataContext.Remove(Name + Variables.previousChild);
            }

            return status;
        }
    }

    /// <summary>
    /// Parallel nodes return SUCCESS if the number of succeeding children is larger than a local constant S (this constant may be different for each parallel node); return FAILURE if the number of failing children is larger than a local constant F; or return RUNNING otherwise.
    /// </summary>
    public class SequenceParallelCompositeNode : CompositeNode
    {
        private class Variables
        {
            public static string Success = "Success";
            public static string Failure = "Failure";
        }

        public SequenceParallelCompositeNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            int successCount = 0;
            int failureCount = 0;
            foreach (BehaviourTreeNode node in Childs)
            {
                BehaviourTreeStatus status = node.Tick(dataContext);
                successCount += Convert.ToInt32(status == BehaviourTreeStatus.Success);
                failureCount += Convert.ToInt32(status == BehaviourTreeStatus.Failure);
            }

            dataContext.Get<int>(Name + Variables.Success, out int minSuccess);
            dataContext.Get<int>(Name + Variables.Failure, out int minFailure);

            if (successCount > minSuccess)
            {
                return BehaviourTreeStatus.Success;
            }
            else if (failureCount > minFailure)
            {
                return BehaviourTreeStatus.Failure;
            }
            else
            {
                return BehaviourTreeStatus.Running;
            }
        }
    }

    public class SelectorCompositeNode : CompositeNode
    {
        private class Variables
        {
            public static string previousChild = "previousChild";
        }

        public SelectorCompositeNode(string name) : base(name) { }

        public override BehaviourTreeStatus Tick(DataContext dataContext)
        {
            dataContext.Get(Name + Variables.previousChild, out int currentChild);

            BehaviourTreeStatus status;
            do
            {
                status = Childs[currentChild].Tick(dataContext);
                if (status == BehaviourTreeStatus.Failure)
                {
                    currentChild++;
                }
            } while (status == BehaviourTreeStatus.Failure && currentChild < Childs.Count);

            if (currentChild < Childs.Count)
            {
                dataContext.Set(Name + Variables.previousChild, currentChild);
            }
            else
            {
                dataContext.Remove(Name + Variables.previousChild);
            }

            return status;
        }
    }
}
