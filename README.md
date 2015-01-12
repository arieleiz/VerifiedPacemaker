# VerifiedPacemaker

This repository is a heart pacemaker implemented on the MBED platform for the UPENN CIS541 course in Fall 2014.

The following features were modelled, implemented and tested as part of the project:

-  Pacemaker - A DDD-R pacemaker using an atrial-based lower rate timing (A-A interval) modelled
   in UPPAAL, and a heart model used for correctness testing. The pacemaker includes several extra
   features not required by the project specification:
   * PVAB timing cycle,
   * PVC support,
   * Dynamic AVI extension,
   * Alarm buzzer,
   * GUI Heart rate visualizer (over serial)
-  The pacemaker was developed as an timed automata model in the UPPAAL language and verified
   for correctness using UPPAAL.
-  Code Generation – A versatile UPPAAL to MBED C++ automatic code generator with direct support
   for binding UPPAAL channels to hardware. The generator accepts UPPAAL XML files directly, and
   supports most of the UPPAAL language. The generator was verified against several models and
   then used to generate code for the pacemaker and heart. The auto-generated code was used with
   no changes, as-is, for the final implementation of both pacemaker and heart.
-  Simulator – A CMSIS OS 2 simulator for Windows was developed for rapid development and testing
   on a PC before deployment to the embedded environment. The simulator does not require any
   code changes in the MBED program, just compiling it using a PC compiler and linking with a stub
   library. This allows for comprehensive testing using tools available for PC and not on embedded
   platforms (builtin fuzzing mechanism, versatile automatic checking, code analysis tools, code
   coverage tools, etc’). The simulator is designed to be extensible by means of a plugin architecture,
   supports multiple MBED programs running concurrently, simulates multiple hardware
   devices/sensors, and supports interconnects between the simulated MBED platforms.
-  Runtime verification and visualization tool – The pacemaker and heart programs output (through
   the serial port) trace logs of signals, senses and paces. A GUI tool was developed to record these
   traces, visualize them as graphs and perform verification on them. In addition, runtime
   verification was implemented by generating a version of the pacemaker with special observer
   templates that check for different runtime properties such as timings.
   
Further information is available in AEPacemaker.pdf.

