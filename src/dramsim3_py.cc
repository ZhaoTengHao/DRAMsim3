#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h> 

#include "memory_system.h"

namespace py = pybind11;

using namespace dramsim3;

class PyCallbackHandler {
  public:
    std::function<void(uint64_t)> read_callback;
    std::function<void(uint64_t)> write_callback;

    void setReadCallback(std::function<void(uint64_t)> callback) {
      read_callback = callback;
    }
    void setWriteCallback(std::function<void(uint64_t)> callback) {
      write_callback = callback;
    }
    void handleRead(uint64_t addr) {
      if (read_callback) {
        read_callback(addr);
      }
    }
    void handleWrite(uint64_t addr) {
      if (write_callback) {
        write_callback(addr);
      }
    }
};

PYBIND11_MODULE(dramsim3_py, m) {
  m.doc() = "DRAMsim3 Python Bindings";

  // bind MemorySystem
  py::class_<MemorySystem>(m, "MemorySystem")
    .def(py::init([](const std::string &config_file,
                    const std::string &output_dir,
                    PyCallbackHandler& cb_handler) {
      return GetMemorySystem(config_file, 
                              output_dir,
                              [&cb_handler](uint64_t addr) { cb_handler.handleRead(addr); },
                              [&cb_handler](uint64_t addr) { cb_handler.handleWrite(addr); });
      }))
    .def("ClockTick", &MemorySystem::ClockTick)
    .def("GetClockCycle", &MemorySystem::GetClockCycle)
    .def("GetTCK", &MemorySystem::GetTCK)
    .def("GetBusBits", &MemorySystem::GetBusBits)
    .def("GetBurstLength", &MemorySystem::GetBurstLength)
    .def("GetQueueSize", &MemorySystem::GetQueueSize)
    .def("WillAcceptTransaction", &MemorySystem::WillAcceptTransaction)
    .def("AddTransaction", &MemorySystem::AddTransaction)
    .def("PrintStats", &MemorySystem::PrintStats)
    .def("ResetStats", &MemorySystem::ResetStats)
    .def("RegisterCallbacks", [](MemorySystem &self, PyCallbackHandler &cb_handler) {
        self.RegisterCallbacks(
            [&cb_handler](uint64_t addr) { cb_handler.handleRead(addr); },
            [&cb_handler](uint64_t addr) { cb_handler.handleWrite(addr); });
      });

  // bind PyCallbackHandler
  py::class_<PyCallbackHandler>(m, "PyCallbackHandler")
    .def(py::init<>())
    .def("setReadCallback", &PyCallbackHandler::setReadCallback)
    .def("setWriteCallback", &PyCallbackHandler::setWriteCallback)
    .def("handleRead", &PyCallbackHandler::handleRead)
    .def("handleWrite", &PyCallbackHandler::handleWrite);
}
