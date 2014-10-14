#include "eva/source.hpp"
#include "eva/stopwatch.hpp"

//#include <boost/python.hpp>
#include <Python.h>

//const int N = 1024*1024;
const int N = 5;

struct test_source : public eva::source
{
    test_source( eva::queue& queue ) :
        eva::source( queue )
    {
}

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            next().get_header()._id = i;
            commit();
        }
    }
};
  
struct python_bridge_guard{
  python_bridge_guard(){
    Py_Initialize();

    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyObject* curDir = PyString_FromString(".");
    PyList_Append(sysPath, curDir);
    Py_DECREF(curDir);
  }

  ~python_bridge_guard(){ Py_Finalize(); }
};

struct python_bridge {
  python_bridge( std::string const& pymodule, std::string const& pyfunc ){
    static python_bridge_guard guard;

    func = NULL;

    PyObject* name = PyString_FromString(pymodule.c_str());
    PyObject* module = PyImport_Import(name);
    Py_DECREF(name);

    if (!module){
    //PyErr_Print();
    //std::cerr << "Error importing " << pymodule << std::endl;
      return;
    }

    func = PyObject_GetAttrString(module, pyfunc.c_str());
    Py_DECREF(module);
    if (!func){
    //PyErr_Print();
    //std::cerr << "Error retrieving " << func << std::endl;
      return;
    }

    if( !PyCallable_Check(func) ){
      Py_DECREF(func);
      func = NULL;
    //PyErr_Print();
    //std::cerr << "Error non callable object " << func << std::endl;
      return;
    }
  }

  virtual ~python_bridge(){
    if(func)
      Py_DECREF(func);
  }

  bool exec( const void* in, size_t in_sz, void* out, size_t out_sz ){
    if (!func){
    //std::cerr << "Error state" << std::endl;
      return  false;
    }

    PyObject* args = PyTuple_New( 2 );
    if (!args){
    //PyErr_Print();
    //std::cerr << "Error building args tuple" << std::endl;
      return false;
    }

    PyObject* in_buff = PyBuffer_FromMemory( (void*)in, in_sz );
    if(!in_buff){
      Py_DECREF(args);
    //PyErr_Print();
    //std::cerr << "Error building in buff" << std::endl;
      return false;
    }

    PyObject* out_buff = PyBuffer_FromReadWriteMemory( out, out_sz );
    if(!out_buff){
      Py_DECREF(in_buff);
      Py_DECREF(args);
    //PyErr_Print();
    //std::cerr << "Error building out buff" << std::endl;
      return false;
    }

    PyTuple_SetItem(args, 0, in_buff);
    PyTuple_SetItem(args, 1, out_buff);

    PyObject* result = PyObject_CallObject(func, args);
//  Py_DECREF(in_buff);
//  Py_DECREF(out_buff);
    Py_DECREF(args);
    if (!result || result != Py_True ){
    //PyErr_Print();
    //std::cerr << "Error invoking function" << std::endl;
      return false;
    }

    Py_DECREF(result);
    return true;
  }

private:
  PyObject* func;
};

struct test_processor : public eva::source
{
    test_processor( eva::queue& inp, eva::queue& out ) :
         eva::source( out ),
        _inp( inp.subscriber() )
    {
    }

    virtual void run()
    {
        python_bridge pb( "processor", "processor" );
        for( int i=0; i<N; i++ )
        {
            // check input event
            const eva::event& e = _inp->next();
            assert( e.get_header()._id == i );

            pb.exec( e.get_raw(), eva::event::size, next().get_raw(), eva::event::size );

            // produce output event
            //next().get_header()._id = i;

            commit();
        }
    }

    eva::subscriber* _inp;
};

struct test_sink : public eva::thread
{
    test_sink( eva::queue& queue ) :
        _inp( queue.subscriber() )
    {
    }

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            const eva::event& e = _inp->next();
            assert( e.get_header()._id == i );

            std::cout << *(int*)e.get_body_buf() << std::endl;
        }
    }

    eva::subscriber* _inp;
};


int main(int argc, char* argv[])
{ 
    eva::stopwatch watch;
    eva::queue* queue_in = new eva::queue();
    eva::queue* queue_out = new eva::queue();

    test_source source( *queue_in );
    test_processor processor( *queue_in, *queue_out );
    test_sink sink( *queue_out );

    watch.start();
    sink.start();
    processor.start();
    source.start();

    sink.join();
    processor.join();
    source.join();

    std::cout << N << " events in " << watch.elapsed_ms() << " ms. " << (int)( N / watch.elapsed_s() ) << " per sec" << std::endl;
}
