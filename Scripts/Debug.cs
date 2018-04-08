using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    class Debug
    {
        public enum LogLevel
        {
            ERROR,
            WARNING,
            INFO
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message, LogLevel logLevel = LogLevel.INFO);
    }
}

