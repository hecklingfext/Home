#region License
/*
MIT License
Copyright © 2006 - 2007 The Mono.Xna Team

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
#endregion License

using System;

namespace Microsoft.Xna.Framework
{
    public enum PowerLineStatus
    {
        Offline,
        Online,
        Unknown
    }

    [Flags]
    public enum BatteryChargeStatus
    {
        Charging = 8,
        Critical = 4,
        High = 1,
        Low = 2,
        NoSystemBattery = 0x80,
        Unknown = 0xff
    }

    public enum PlayerIndex
    {
        One,
        Two,
        Three,
        Four
    }

    public enum CurveLoopType
    {
        Constant,
        Cycle,
        CycleOffset,
        Oscillate,
        Linear
    }

    public enum CurveContinuity
    {
        Smooth,
        Step
    }

    public enum CurveTangent
    {
        Flat,
        Linear,
        Smooth
    }

    public enum TargetPlatform
    {
        Unknown,
        Windows,
        Xbox360,
        Zune
    }

    public enum PlaneIntersectionType
    {
        Front,
        Back,
        Intersecting
    }

    public enum ContainmentType
    {
        Disjoint,
        Contains,
        Intersects
    }
}
