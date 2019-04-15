## Top-level project
project('libtdd', 'c',
        version: '1.0.0',
        license: 'ISC',
        default_options: [
            'c_std=c99',
        ])

posix_c_source = get_option('posix_c_source')

add_global_arguments('-D_X_POSIX_C_SOURCE='+posix_c_source,
                     language: 'c')

use_colour_output = get_option('use_colour_output')
if use_colour_output
    add_global_arguments('-DUSE_COLOUR', language: 'c')
endif

threads = dependency('threads')

project_sources = []
project_api_headers = []
project_headers = []
project_includes = []
project_objects = []
example_sources = []

subdir('src')       # populate project_sources
subdir('include')   # populate project_includes
subdir('docs')      # create documentation

project_headers += project_api_headers

install_headers(project_api_headers)
lib = library('tdd', install: true,
              sources: project_sources,
              include_directories: project_includes,
              dependencies: threads)

configure_file(input: '.clang-format',
               output: '.clang-format',
               copy: true)
run_target('format',
            command: [
                'clang-format', '-i', '-style=file',
                project_sources,
                project_headers,
                example_sources
           ])

subdir('examples')
executable('example', example_sources,
           include_directories: project_includes,
           link_with: lib)
