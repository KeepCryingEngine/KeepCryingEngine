using System;
using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    class InstanceManager
    {
        public static GameObject InstantiateCube(float x, float y, float z)
        {
            GameObject cube = new GameObject(InstantiateCubeInternal());
            cube.GetTransform().Translate(x,y,z);
            return cube;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Int32 InstantiateCubeInternal();
    }
}
