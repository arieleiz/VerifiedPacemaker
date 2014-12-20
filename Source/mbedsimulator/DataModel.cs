/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using mbedsimulatortypes;

namespace mbedsimulator
{
    class DataModel
    {
        public DataModel()
        {
        }

        public void add_info(MbedProgramInfo mpi)
        {
            _infos.Add(mpi);
        }

        public MbedProgramInfo[] Infos
        {
            get
            {
                return _infos.ToArray();
            }
        }

        private List<MbedProgramInfo> _infos = new List<MbedProgramInfo>();
    }
}
