using System;
using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    public class GameObject
    {
        Int32 id;
        private Transform transform = null;

        public GameObject(Int32 id)
        {
            this.id = id;
        }

        public Transform GetTransform()
        {
            if(transform == null)
            {
                transform = new Transform(GetTransform(id));
            }
            return transform;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Int32 GetTransform(Int32 gameObjectPtr);
    }
}
