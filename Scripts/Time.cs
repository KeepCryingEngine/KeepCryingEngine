using System;
using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    class Time
    {

        public static float GetDeltaTime()
        {
            return GetDeltaTimeInternal();
        }

        public static float GetRealDeltaTime()
        {
            return GetRealDeltaTimeInternal();
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float GetDeltaTimeInternal();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float GetRealDeltaTimeInternal();
    }
}
