from setuptools import setup, find_packages
import datetime, os, re, subprocess


setup(

    name='wenet',
    version="0.0.1",
    description='wenet runtime python binding',
    long_description_content_type='text/markdown',
    author='Mddct',
    # For a list of valid classifiers, see https://pypi.org/classifiers/
    classifiers=[
        # How mature is this project? Common values are
        #   3 - Alpha
        #   4 - Beta
        #   5 - Production/Stable
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU Affero General Public License v3',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
    ],
    # keywords='speech recognition',
    packages=[''],
    package_data={"wenet":["lib/*"]},
    zip_safe=False,
    python_requires='>=3.6, <4',
)
