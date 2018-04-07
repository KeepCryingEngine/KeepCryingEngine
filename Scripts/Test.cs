using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class Test
{
    public void TestMethod(Int32 transform)
    {
       Transform t = new Transform(transform);

       t.Translate(1, 0, 0);
    }
}

