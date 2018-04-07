﻿using System;
using System.Runtime.CompilerServices;

namespace KeepCryingEngine
{
    public class Transform
    {
        Int32 cPtr;

        public Transform(Int32 cPtr)
        {
            this.cPtr = cPtr;
        }

        public void Translate(float x, float y, float z)
        {
            Console.Write("Before");
            Translate(cPtr, x, y, z);
            Console.Write("After");
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void Translate(Int32 intPtr, float x, float y, float z);
    }
}