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
using System.Windows.Forms;
using System.Xml;
using System.IO;
using mbedsimulatortypes;

namespace mbedsimulator
{
    class DataLoader
    {
        public DataLoader()
        {

        }

        public bool parseXml(string xml)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(xml);

                XmlNodeList mbeds = doc.SelectNodes("/mbedsimulator/mbed");
                if(mbeds != null)
                {
                    foreach(XmlNode node in mbeds)
                    {
                        MbedProgramInfo info = new MbedProgramInfo();

                        XmlElement elm = (XmlElement) node;
                        info.name = elm.GetAttribute("name");
                        info.dllname = elm.GetAttribute("dll");
                        info.vizname = elm.GetAttribute("viz");
                        if(!string.IsNullOrEmpty(info.vizname))
                        {
                            if (!Path.IsPathRooted(info.vizname))
                                info.vizname = Path.Combine(Path.GetDirectoryName(xml), info.vizname);
                        }
                        info.has_display = false;
                        info.has_serial = false;
                        if (elm.SelectSingleNode("./textlcd") != null)
                            info.has_display = true;
                        if (elm.SelectSingleNode("./serial") != null)
                            info.has_serial = true;
                        List<Device> devices = new List<Device>();
                        XmlNodeList subnodes = elm.ChildNodes;
                        if (subnodes != null)
                            devices.AddRange((from XmlNode n in subnodes select createDevice((XmlElement)n)).Where(d => d.type != DeviceType.Unknown));

                        info.devices = devices.ToArray();
                        Model.add_info(info);
                    }
                }
                return true;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
        }

        private mbedsimulatortypes.Device createDevice(XmlElement node)
        {
            Device d = new Device();
            switch (node.Name.ToLower())
            {
                case "led":
                    d.type = DeviceType.Led;
                    break;
                case "pushbutton":
                    d.type = DeviceType.PushButton;
                    break;
                case "switch":
                    d.type = DeviceType.Switch;
                    break;
                case "crossover":
                    d.type = DeviceType.Crossover;
                    break;
                case "ground":
                    d.type = DeviceType.Ground;
                    break;
                case "vout":
                    d.type = DeviceType.Vout;
                    break;

                case "textlcd":
                    break;

                case "serial":
                    break;

                default:
                    throw new Exception("Unknown device type: " + node.Name);
            }

            if (d.type != DeviceType.Unknown)
            {
                d.name = node.GetAttribute("name");
                d.extra = node.GetAttribute("extra");

                d.pin = (PinName)Enum.Parse(typeof(PinName), node.GetAttribute("pin"), true);

            }
            return d;
        }

        public DataModel Model { get { return _datamodel;  } }

        private DataModel _datamodel = new DataModel();

    }
}
