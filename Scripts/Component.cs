namespace KeepCryingEngine
{
    public class Component
    {
        public GameObject gameObject;

        public void InitMonoBehaviour(GameObject gameObject)
        {
            this.gameObject = gameObject;
        }

        public virtual void Awake() { }
        public virtual void Start() { }

        public virtual void Update() { }
        public virtual void Destroy() { }
    }
}