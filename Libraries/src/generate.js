const { opendirSync, readFileSync } = require('fs');


function DetermineSourceFiles() {
	const sourceFiles = [];
	let dir = opendirSync('.');
	for (let dirent = dir.readSync(); dirent != null; dirent = dir.readSync()) {
		if (dirent.isFile() && /.*\.hpp$/.test(dirent.name)) {
			sourceFiles.push(dirent.name)
		}
	}
	sourceFiles.sort();
	return sourceFiles;
}


function LoadFile(filename) {
	return readFileSync(filename).toString();
}


function GetIncludeSection(source) {
	const regex = /\/\*\s@INCLUDE\s\*\/([\s\S]*)\/\*\s@HEADER\s\*\//m
	const matches = source.match(regex);
	if (matches) {
		// first group
		return matches[1];
	}
}


function GetIncludeSet(source) {
	const section = GetIncludeSection(source);
	const lines = 
}


function GetHeaderSection(source) {
	const regex = /\/\*\s@HEADER\s\*\/([\s\S]*)\/\*\s@IMPLEMENTATION\s\*\//m
	const matches = source.match(regex);
	if (matches) {
		// first group
		return matches[1];
	}
}


function GetImplementationSection(source) {
	const regex = /\/\*\s@IMPLEMENTATION\s\*\/([\s\S]*)/m
	const matches = source.match(regex);
	if (matches) {
		// first group
		return matches[1];
	}
}


function BuildFile(sourceFiles) {
	let headers = [];
	let implementations = [];

	for (let filename of sourceFiles) {
		const source = LoadFile(filename);
		headers.push(GetHeaderSection(source));
		implementations.push(GetImplementationSection(source));
	}

	const file = `#pragma once

/*******************************************************************************
 *
 * smm.h
 *
 * This is an automatically generated file - please do not edit it directly!
 *
 *******************************************************************************
 */

${headers.join('\n\n')}

#if defined(SMM_IMPLEMENTATION)
${implementations.join('\n\n')}
#endif
`;
	return file;
}


const sources = DetermineSourceFiles();
console.log(BuildFile(sources));
