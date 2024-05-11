//
//  Plugin.h
//  Effect & Synth Plugin Framework - Mini-Plugin Wrappers
//
//  Created by Chris Nash on 02/10/2013.
//  Updated by Chris Nash on 18/01/2016 with support for OS X 10.10 SDK.
//  Updated by Chris Nash on 02/10/2018 with support for TestEffectAU.
//  Updated by Chris Nash on 02/09/2020 with new architecture for MacOS and Windows.
//  Updated by Chris Nash on 10/08/2022 with Klang support and Base64 backgrounds.
//
//  This file describes a number of abstractions and extensions to STK,
//  to support audio processing and programming teaching at UWE.

#pragma once

#define STR(x) #x
#define FILE(NAME,EXT) STR(NAME.EXT)

#define PLUGIN_K FILE(PLUGIN_NAME,k)

#include <string>

#if __has_include("Background")
#include "Background"
#else
#define BACKGROUND 0
#pragma message("Background not found")
#endif

namespace Background {
    const char data[] = { 
        BACKGROUND 
    };
    const int size = sizeof(data);
}

namespace klang { struct Graph; struct Console; }

#if defined(_WIN32)
#define PTR_FUNCTION   __declspec(dllexport) void* __stdcall
#define INT_FUNCTION   __declspec(dllexport) int __stdcall
#define VOID_FUNCTION  __declspec(dllexport) void __stdcall
#else
#define PTR_FUNCTION   void*
#define INT_FUNCTION   int
#define VOID_FUNCTION  void
#endif

#if KLANG

#define CREATE_EFFECT(name)                                                                                                 \
    PTR_FUNCTION effectCreate(float sampleRate) {                                                                           \
        klang::fs = sampleRate;                                                                                             \
        ::stk::Stk::setSampleRate(sampleRate);                                                                              \
        return (DSP::Effect*)new KEffect(new name());                                                                       \
    }                                                                                                                       

#define CREATE_SYNTH(name)                                                                                                  \
    PTR_FUNCTION synthCreate(float sampleRate) {                                                                            \
        klang::fs = sampleRate;                                                                                             \
        ::stk::Stk::setSampleRate(sampleRate);                                                                              \
        return (DSP::Synth*)new KSynth(new name());                                                                         \
    }

#else

#define CREATE_EFFECT(name)                                                                                                 \
    PTR_FUNCTION effectCreate(float sampleRate) {                                                                           \
        ::stk::Stk::setSampleRate(sampleRate);                                                                              \
        return (DSP::Effect*)new name();                                                                                    \
    }                                                                                                                       

#define CREATE_SYNTH(name)                                                                                                  \
    PTR_FUNCTION synthCreate(float sampleRate) {                                                                            \
        ::stk::Stk::setSampleRate(sampleRate);                                                                              \
        return (DSP::Synth*)new name();                                                                                     \
    }

#endif

#define EFFECT(name)                                                                                                        \
extern "C" {                                                                                                                \
    CREATE_EFFECT(name)                                                                                                     \
                                                                                                                            \
    VOID_FUNCTION getBackground(void** data, int* size) {                                                                   \
        *data = (void*)Background::data;                                                                                    \
        *size = Background::size;                                                                                           \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION getDebugData(void* effect, const float** const buffer, int* size, void** graph, void** console) {          \
        try {                                                                                                               \
            if(buffer) *size = ((DSP::Effect*)effect)->getDebugAudio(buffer);                                               \
            if(graph) ((DSP::Effect*)effect)->getDebugGraph(graph);                                                         \
            if(console) ((DSP::Effect*)effect)->getDebugConsole(console);                                                   \
            return 0;                                                                                                       \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    VOID_FUNCTION effectDestroy(void* effect) {                                                                             \
        delete (DSP::Effect*)effect;                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION effectProcess(void* effect, const float** inputBuffers, float** outputBuffers, int numSamples) {           \
        try { ((DSP::Effect*)effect)->process(inputBuffers, outputBuffers, numSamples); return 0;                           \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
}

#define SYNTH(name)                                                                                                         \
extern "C" {                                                                                                                \
    CREATE_SYNTH(name)                                                                                                      \
                                                                                                                            \
    VOID_FUNCTION synthDestroy(void* synth) {                                                                               \
        delete (DSP::Synth*)synth;                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    VOID_FUNCTION getBackground(void** data, int* size) {                                                                   \
        *data = (void*)Background::data;                                                                                    \
        *size = Background::size;                                                                                           \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION getDebugData(void* synth, const float** const buffer, int* size, void** graph, void** console) {           \
        try {                                                                                                               \
            if(buffer) *size = ((DSP::Synth*)synth)->getDebugAudio(buffer);                                                 \
            if(graph) ((DSP::Synth*)synth)->getDebugGraph(graph);                                                           \
            if(console) ((DSP::Effect*)effect)->getDebugConsole(console);                                                   \
            return 0;                                                                                                       \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION noteOnStart(void* note, int pitch, float velocity){                                                        \
        try { ((DSP::Note*)note)->onStartNote(pitch, velocity); return 0;                                                   \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION noteOnStop(void* note, float velocity, bool* hasRelease = NULL){                                           \
        try { bool terminate = ((DSP::Note*)note)->onStopNote(velocity);                                                    \
              if (hasRelease) *hasRelease = !terminate;                                                                     \
              return 0;                                                                                                     \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION noteOnPitchWheel(void* note, int value){                                                                   \
        try { ((DSP::Note*)note)->onPitchWheel(value); return 0;                                                            \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION noteOnControlChange(void* note, int controller, int value){                                                \
        try { ((DSP::Note*)note)->onControlChange(controller, value); return 0;                                             \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION noteProcess(void* note, float** outputBuffers, int numSamples, bool* shouldContinue = NULL) {              \
        try { bool bContinue = ((DSP::Note*)note)->process(outputBuffers, 2, numSamples);                                   \
              if(shouldContinue) *shouldContinue = bContinue;                                                               \
              return 0;                                                                                                     \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
                                                                                                                            \
    INT_FUNCTION synthProcess(void* synth, const float** inputBuffers, float** outputBuffers, int numSamples) {             \
        try { ((DSP::Synth*)synth)->process(inputBuffers, outputBuffers, numSamples); return 0;                             \
        } catch(...) { return 1; }                                                                                          \
    }                                                                                                                       \
}

namespace DSP
{
    class Wavetable;

	template<int SIZE>
    struct Text {
        char string[SIZE + 1] = { 0 };

        operator const char* () const { return string; }
        const char* c_str() const { return string; }

		static Text from(const char* in) {
			Text text;
			text = in;
			return text;
		}

        void operator=(const char* in) {
            memcpy(string, in, SIZE);
            string[SIZE] = 0;
        }
    };

    typedef Text<32> Caption;

    struct Parameter
    {
        enum Type
        {
			NONE,	// no control (list terminator)
            ROTARY,	// rotary knob (dial/pot)
            BUTTON, // push button (trigger)
            TOGGLE, // on/off switch (toggle)
            SLIDER, // linear slider (fader)
            MENU,   // drop-down list (menu; up to 128 items)
            METER,  // level meter (read-only: use setParameter() to set value)
            WHEEL,  // MIDI control (Pitch Bend / Mod Wheel only)
        };

		struct Size
        {
            Size(int x = -1, int y = -1, int width = -1, int height = -1)
            : x(x), y(y), width(width), height(height) { }
        
            int x;
            int y;
            int width;
            int height;

            bool isAuto() const { return x == -1 && y == -1 && width == -1 && height == -1;  }
        };

        template<typename TYPE, int CAPACITY>
        struct Array {
            TYPE items[CAPACITY];
            unsigned int count = 0;

            unsigned int size() const { return count; }

            void add(const TYPE& item) {
                if (count < CAPACITY)
                    items[count++] = item;
            }

            void clear() {
                count = 0;
            }

            TYPE& operator[](int index) { return items[index]; }
            const TYPE& operator[](int index) const { return items[index]; }
        };

        typedef Array<Caption, 128> Options;

        Caption name;           // name for control label / saved parameter
        Type type = NONE;       // control type (see above)
    
        float min;              // minimum control value (e.g. 0.0)
        float max;              // maximum control value (e.g. 1.0)
        float initial;          // initial value for control (e.g. 0.0)
    
        Size size;              // position (x,y) and size (height, width) of the control (use AUTO_SIZE for automatic layout)
        Options options;        // text options for menus and group buttons

        float value;            // current control value;
    };

	const Parameter::Size Automatic = { -1, -1, -1, -1 };
	const Parameter::Options NoOptions;

	static Parameter Dial(const char* name, float min = 0.f, float max = 1.f, float initial = 0.f)
	{	return { Caption::from(name), Parameter::ROTARY, min, max, initial, Automatic, NoOptions, initial };	}

	static Parameter Button(const char* name)
	{	return { Caption::from(name), Parameter::BUTTON, 0, 1, 0.f, Automatic, NoOptions, 0.f };	}

	static Parameter Toggle(const char* name, bool initial = false)
	{	return { Caption::from(name), Parameter::TOGGLE, 0, 1, initial ? 1.f : 0.f, Automatic, NoOptions, initial ? 1.f : 0.f};	}

	static Parameter Slider(const char* name, float min = 0.f, float max = 1.f, float initial = 0.f)
	{	return { Caption::from(name), Parameter::SLIDER, min, max, initial, Automatic, NoOptions, initial }; }

	template<typename... Options>
	static Parameter Menu(const char* name, const Options... options)
	{	Parameter::Options menu;
	    const char* strings[] = { options... };
        int nbValues = sizeof...(options);
        for(int p=0; p<nbValues; p++)
            menu.add(Caption::from(strings[p]));
		return { Caption::from(name), Parameter::MENU, 0, menu.size() - 1.f, 0, Automatic, menu, 0 }; 
	}

	static Parameter Meter(const char* name, float min = 0.f, float max = 1.f, float initial = 0.f)
	{	return { Caption::from(name), Parameter::METER, min, max, initial, Automatic, NoOptions, initial }; }

	static Parameter PitchBend()
	{	return { { "PITCH\nBEND" }, Parameter::WHEEL, 0.f, 16384.f, 8192.f, Automatic, NoOptions, 8192.f }; }

	static Parameter ModWheel()
	{	return { { "MOD\nWHEEL" }, Parameter::WHEEL, 0.f, 127.f, 0.f, Automatic, NoOptions, 0.f }; }

    struct Parameters : Parameter::Array<Parameter, 128>
    {
		void operator+= (const Parameter& control) {
			items[count++] = control;
		}

		void operator= (const Parameters& controls) {
			for (int c = 0; c < 128 && controls(c).type != Parameter::NONE; c++)
				operator+=(controls(c));
		}

        void add(const char* name, Parameter::Type type = Parameter::ROTARY, float min = 0.f, float max = 1.f, float initial = 0.f, Parameter::Size size = Automatic) {
            items[count].name = name;
            items[count].type = type;
            items[count].min = min;
            items[count].max = max;
            items[count].initial = initial;
            items[count].size = size;
            items[count++].value = initial;
        }

        float& operator[](int index) { return items[index].value; }
        float operator[](int index) const { return items[index].value; }

        Parameter& operator()(int index) { return items[index]; }
        Parameter operator()(int index) const { return items[index]; }
    };

    struct Preset {
        Caption name;
        Parameter::Array<float, 128> values;
    };

	struct Presets : Parameter::Array<Preset, 128> {
		void operator += (const Preset& preset) {
			items[count++] = preset;
		}

        template<typename... Values>
        void add(const char* name, const Values... values) {
            items[count].name = name;

            const float preset[] = { values... };
            int nbValues = sizeof...(values);
            for(int p=0; p<nbValues; p++)
                items[count].values.add(preset[p]);
            count++;
        }
    };

    struct Debug {
        const float* buffer = nullptr;
        klang::Graph* graph = nullptr;
        klang::Console* console = nullptr;
        int size = 0;

        void reset() { buffer = nullptr; graph = nullptr; console = nullptr; size = 0; }
    };

    class Effect
    {
    public:
        Effect() { }
        virtual ~Effect() { }
        
        virtual void process(const float** inputBuffers, float** outputBuffers, int numSamples) = 0;
        
        virtual void presetLoaded(int iPresetNum, const char *sPresetName) { };
        virtual void optionChanged(int iOptionMenu, int iItem) { };
        virtual void buttonPressed(int iButton) { };
        
        virtual void setSampleRate(float sr) = 0;
        virtual float getSampleRate() const = 0;

        virtual int getDebugAudio(const float** buffer) const {
            if (buffer)
                *buffer = debug.buffer;
            return debug.size;
        }

        virtual void getDebugGraph(void** graph) const {
            if (graph)
                *graph = debug.graph;
        }

        virtual void getDebugConsole(void** console) const {
            if (console)
                *console = debug.console;
        }
        
        Parameters parameters;
        Presets presets;

        Debug debug;
    };

    class Note;

    class Synth : public Effect
    {
    public:
        Synth()
        : wavetables(nullptr) { }
        
        Synth(const char* resources) : wavetables(nullptr) {
            try { if(resources) loadWavetables(resources); } catch (...) { };
        }
        
        virtual ~Synth() {
            try { unloadWavetables(); } catch (...) { };
        }
               
        Note* notes[128] = { 0 };
        const Wavetable* const getWavetable(int index) const;
        
    private:
        Wavetable* wavetables;
        void loadWavetables(const char* path);
        void unloadWavetables();
    };

    class Note
    {
    public:
        Note(Synth* synth) : parameters(synth->parameters), presets(synth->presets), synth(synth) { }
        virtual ~Note() { }
            
        float getSampleRate() const { return synth->getSampleRate(); }
            
        virtual void onStartNote (int pitch, float velocity) = 0;
        virtual bool onStopNote (float velocity) = 0;
            
        virtual void onPitchWheel (const int value) { };
        virtual void onControlChange (const int controller, const int value) { };
            
        virtual bool process (float** outputBuffer, int numChannels, int numSamples) = 0;
                
    protected:
        Parameters& parameters;
        Presets& presets;
        Synth* synth;
    };

} // namespace DSP
