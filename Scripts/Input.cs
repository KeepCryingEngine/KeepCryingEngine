using System;
using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    enum KeyCode {A}

    enum KeyState {
        KEY_IDLE,
        KEY_DOWN,
        KEY_REPEAT,
        KEY_UP
    }

    class Input
    {

        public static KeyState GetKey(KeyCode key)
        {
            return GetKeyInternal(key);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static KeyState GetKeyInternal(KeyCode key);
    }
}
