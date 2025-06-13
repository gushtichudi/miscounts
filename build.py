import subprocess as sp

from typing_extensions import TextIO
from typing import Any
from enum import Enum

import sys

class Messages:
    class Prefix(Enum):
        CompilerMessage = 0
        Meta            = 1
        CompilerError   = 2

    def __init__(self, redirect_output: Any = None):
        self.redirect_output = redirect_output

    def put_message(self, kind: Prefix, msg: str) -> None:
        match kind:
            case Messages.Prefix.CompilerMessage:
                if not self.redirect_output:
                    sys.stderr.write(f"[CC] {msg}\n")
                    return

                with open(self.redirect_output, 'a') as file:
                    file.write(f"[CC] {msg}\n")
                    return

            case Messages.Prefix.Meta:
                if not self.redirect_output:
                    sys.stderr.write(f"[!!] {msg}\n")
                    return

                with open(self.redirect_output, 'a') as file:
                    file.write(f"[!!] {msg}\n")
                    return

            case Messages.Prefix.CompilerError:
                if not self.redirect_output:
                    sys.stderr.write(f"[EE] {msg}\n")
                    return

                with open(self.redirect_output, 'a') as file:
                    file.write(f"[EE] {msg}\n")
                    return

class BinaryType(Enum):
    Program = 0
    Object  = 1
    SharedObject = 2

class Build:
    def __init__(self, program_name):
        self.program_name = program_name
        self.compiler = "cc" # by default
        self.global_cc_flags: list = []   # every file has it

        self.task_queue: dict = {}
        self.task_queue_index: int = 0

        self.message = Messages()
        self.stderr = sys.stderr
        self.stderr_changed = False

    def redirect_stderr(self, filename) -> None:
        self.message.put_message(Messages.Prefix.Meta, f"Redirecting stderr to {filename}")
        self.stderr = open(filename, 'w')
        self.stderr_changed = True

    @staticmethod
    def repurpose_stderr(stderr: TextIO | Any, mode: str = "a") -> Any:
        if type(stderr) == str:
            return open(stderr, mode)

        if not stderr.closed:
            stderr.close()
            return open(stderr.name, mode)

    @staticmethod
    def get_right_file_extension() -> str | None:
        match sys.platform:
            # ELF
            case 'linux': return '.so'
            case 'freebsd': return '.so'
            # PE
            case 'win32': return '.dll'
            case 'cygwin': return '.dll'
            # MACH-O
            case 'darwin': return '.dylib'

            # NOTHING ELSE...
            case _:
                raise OSError("I don't know what are you using. Please tell me what you are using in GitHub Issues(TM).")

    def override_default_compiler(self, compiler_name: str) -> None:
        self.compiler = compiler_name

    def add_compiler_arguments(self, arguments: str | list) -> None:
        if type(arguments) == str:
            self.global_cc_flags.append(arguments)
            return

        for argument in arguments:
            self.global_cc_flags.append(argument)

    def add_task_queue(self, arguments: list) -> None:
        self.task_queue[str(self.task_queue_index)] = arguments
        self.task_queue_index += 1

    def add_file(
        self,
        file: str | list,
        dependencies: str | list | None = None,
        is_what: BinaryType | None = None
    ) -> None:
        if not dependencies:
            dependencies = ""

        match is_what:
            case BinaryType.Program:
                for files in file:
                    self.add_task_queue(
                        list(
                            [
                                self.compiler,
                                "-o",
                                self.program_name,
                                files,
                            ] + list(dependencies) + list(self.global_cc_flags)
                        )
                    )

            case BinaryType.Object:
                for files in file:
                    self.add_task_queue(
                        list(
                            [
                                self.compiler,
                                "-c",
                                files,
                                "-o",
                                str(files.split("/")[-1].split('.')[0]) + str('.o'),
                            ] + list(dependencies) + list(self.global_cc_flags)
                        )
                    )

            case BinaryType.SharedObject:
                shared_object_file_extension = Build.get_right_file_extension()

                for files in file:
                    self.add_task_queue(
                        list(
                            [
                                self.compiler,
                                "-shared",
                                files,
                                "-o",
                                str(files.split('.')[0]) + str(shared_object_file_extension),
                            ] + list(dependencies) + list(self.global_cc_flags)
                        )
                    )

            case _:
                raise Exception("Invalid Binary Type: {}".format(repr(is_what)))

    def add_resource(self, resource_intepreter: str, resource_intepreter_arguments: list) -> None:
        self.add_task_queue([resource_intepreter, resource_intepreter_arguments])

    # TODO: time-based building so we don't rebuild the entire thing
    def start_build(self):
        for task_queues in reversed(self.task_queue):
            command_line = self.task_queue[task_queues]
            try:
                self.message.put_message(Messages.Prefix.CompilerMessage,
                "queue:{} >> {}".format(task_queues, " ".join(command_line)))
            except TypeError as e:
                self.message.put_message(Messages.Prefix.Meta,
                    "ERROR while trying to represent command-line: `{}`".format(e))

                exit(-1)

            process = sp.Popen(
                command_line, stdout=sp.PIPE, stderr=self.stderr
            )

            # wait for process so we get return code
            process.wait()

            if process.returncode != 0:
                # btw, we still have to read from self.stderr...
                if not self.stderr_changed:
                    self.message.put_message(Messages.Prefix.CompilerError, "Compilation failed!")

                    return

                self.message.put_message(Messages.Prefix.CompilerError, "Compilation failed!")

                # close and reopen self.stderr for reading instead
                self.stderr = Build.repurpose_stderr(self.stderr, 'r')
                self.stderr.close()

                return

            continue

        self.message.put_message(Messages.Prefix.Meta, "---- Compilation finished ----")
