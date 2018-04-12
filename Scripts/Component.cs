using System;

namespace KeepCryingEngine
{
    public class Component
    {
        public GameObject gameObject;

        public void InitMonoBehaviour(Int32 gameObjectId)
        {
            this.gameObject = new GameObject(gameObjectId);
        }

        public virtual void Awake() { }
        public virtual void Start() { }

        public virtual void Update() { }
        public virtual void Destroy() { }
    }
}