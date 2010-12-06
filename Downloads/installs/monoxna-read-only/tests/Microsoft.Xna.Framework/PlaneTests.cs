#region License
/*
MIT License
Copyright � 2006-2007 The Mono.Xna Team

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#endregion

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Microsoft.Xna.Framework;

namespace Microsoft.Xna.Framework.Tests
{
    [TestFixture]
    public class PlaneTests
    {
		#region Setup
		private Plane p;

		[SetUp]
		public void Setup()
		{
			p = new Plane(new Vector3(1f, 2f, 3f), 4f);
		}
		#endregion Setup

        [Test]
        public void ToStringTest()
        {
			Assert.AreEqual("{Normal:{X:1 Y:2 Z:3} D:4}", p.ToString(), "#1");
        }

        [Test]
        public void EqualsTest()
        {
			Assert.IsTrue(p.Equals(p),"#1");
			Assert.IsTrue(p.Equals(new Plane(new Vector3(1f, 2f, 3f), 4f)), "#2");			
        }

		//do not need it all intersect test ever done in other class
		//[Test]
		//public void IntersectsTest()
		//{
		//    throw new NotImplementedException();
		//}
    }
}
