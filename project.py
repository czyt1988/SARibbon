import os
import sys

from pyqtbuild import PyQtBindings, PyQtProject
from sipbuild import Option


class PyQtSARibbon(PyQtProject):
    def __init__(self):
        """ Initialise the project. """
        super().__init__()
        self.bindings_factories = [saribbon]


class saribbon(PyQtBindings):
    def __init__(self, project):
        """ Initialise the bindings. """
        super().__init__(project, 'saribbon')

    def get_options(self):
        """Our custom options that a user can pass to sip-build."""
        options = super().get_options()
        options += [
            Option('saribbon_incdir',
                   help='the directory containing the SARibbon header files',
                   metavar='DIR'),
            Option('saribbon_libdir',
                   help='the directory containing the SARibbon library',
                   metavar='DIR'),
            Option('saribbon_lib',
                   help='the SARibbon library name',
                   metavar='LIB'),
        ]
        return options

    def apply_user_defaults(self, tool):
        """ Set default values for user options that haven't been set yet. """
        if self.saribbon_lib is not None:
            self.libraries.append(self.saribbon_lib)

        if self.saribbon_incdir is not None:
            self.include_dirs.append(self.saribbon_incdir)

        if self.saribbon_libdir is not None:
            self.library_dirs.append(self.saribbon_libdir)

        super().apply_user_defaults(tool)
