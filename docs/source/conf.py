# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
from sphinx.builders.html import StandaloneHTMLBuilder
import subprocess, os

# Doxygen
subprocess.call('doxygen ../Doxyfile.in', shell=True)
#subprocess.call('ls', shell=True)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'tsudoku'
copyright = '2023, Chris Swindell'
author = 'Chris Swindell'
release = '1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['myst_parser', 'breathe']

templates_path = ['_templates']
exclude_patterns = ['doxygen_docs']
#c_autodoc_roots = ['../../src']
highlight_language = 'c++'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'scrolls'
html_static_path = ['_static']

# Breath configuration
breathe_projects = {
    'tsudoku': 'doxygen_docs/xml'
}
#breathe_projects_source = {
#    'auto': ('../doxygen_docs/xml', ['main_8cpp.xml'])
#}
breathe_default_project = 'tsudoku'
breathe_default_members = ('members', 'undoc-members')
