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
using System.IO;

namespace model2mbed
{
    class Program
    {
        static void Main(string[] args)
        {
            if(args.Length < 2)
            {
                System.Console.WriteLine("Syntax: model2mbed <a.xml> <outdir> [template1 [template2 ...]]");
                System.Console.WriteLine("Syntax: model2mbed <a.xml> <outdir> <@template.lst>");
                return;
            }

            try
            {
                List<string> templates = new List<string>();
                for(int i = 2; i < args.Length; ++ i)
                {
                    if (args[i].StartsWith("@"))
                        templates.AddRange(File.ReadAllLines(args[i].Substring(1)).Where(s => !string.IsNullOrWhiteSpace(s)));
                    else
                        templates.Add(args[i]);
                }
                Model m = UPAALParser.Parse(args[0], templates);
                ReferenceOptimizer.cleanup(m);
                Generator g = new Generator(m);
                g.generate(args[1]);
            }
            catch(Exception e)
            {
                System.Console.WriteLine("Error: " + e.Message);
                System.Console.WriteLine("At:");
                System.Console.WriteLine(e.StackTrace);
            }
        }


    }
}
