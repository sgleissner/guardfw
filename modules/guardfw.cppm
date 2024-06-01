/*
 * Primary module interface for Guard Framework
 *
 * The WrapperError exception is based on std::system_error and contains both the name of the
 * failed wrappers API call and the position of the failed call (probably of the wrapper call).
 *
 * @author    Simon Gleissner <simon@gleissner.de>
 * @copyright MIT license, see file LICENSE
 * @file
 */

export module guardfw;

export import guardfw.config;  // cmake-generated module, may not be found by IDE
export import guardfw.exceptions;
export import guardfw.file_desciptor;
export import guardfw.traits;
export import guardfw.wrapper;

export import guardfw.wrapped_eventfd;
export import guardfw.wrapped_fcntl;
export import guardfw.wrapped_ioctl;
export import guardfw.wrapped_mman;
export import guardfw.wrapped_mqueue;
export import guardfw.wrapped_resource;
export import guardfw.wrapped_signal;
export import guardfw.wrapped_signalfd;
export import guardfw.wrapped_socket;
export import guardfw.wrapped_stdio;
export import guardfw.wrapped_timerfd;
export import guardfw.wrapped_unistd;
