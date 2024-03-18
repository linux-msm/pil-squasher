// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2019, Linaro Ltd. All rights reserved.
 */
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

static void usage(void)
{
	extern const char *__progname;

	fprintf(stderr, "%s: <mbn output> <mdt header>\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	unsigned char e_ident[EI_NIDENT];
	unsigned int phnum;
	size_t phoff;
	bool is_64bit;
	size_t offset;
	void *segment;
	ssize_t n;
	char *ext;
	int mdt;
	int mbn;
	int bxx;
	int i;

	if (argc != 3)
		usage();

	ext = strstr(argv[2], ".mdt");
	if (!ext)
		errx(1, "%s doesn't end with .mdt", argv[2]);

	mbn = open(argv[1], O_RDONLY);
	if (mbn < 0)
		err(1, "failed to open %s", argv[1]);

	mdt = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (mdt < 0)
		err(1, "failed to open %s", argv[2]);

	read(mbn, e_ident, sizeof(e_ident));
	if (memcmp(e_ident, ELFMAG, SELFMAG))
		errx(1, "not an ELF file %s", argv[2]);

	if (e_ident[EI_CLASS] == ELFCLASS32) {
		Elf32_Ehdr ehdr;

		is_64bit = false;
		pread(mbn, &ehdr, sizeof(ehdr), 0);
		pwrite(mdt, &ehdr, sizeof(ehdr), 0);
		phoff = ehdr.e_phoff;
		phnum = ehdr.e_phnum;
	} else if (e_ident[EI_CLASS] == ELFCLASS64) {
		Elf64_Ehdr ehdr;

		is_64bit = true;
		pread(mbn, &ehdr, sizeof(ehdr), 0);
		pwrite(mdt, &ehdr, sizeof(ehdr), 0);
		phoff = ehdr.e_phoff;
		phnum = ehdr.e_phnum;
	} else
		errx(1, "Unsupported ELF class %d", e_ident[EI_CLASS]);

	for (i = 0; i < phnum; i++) {
		size_t p_filesz, p_offset;
		unsigned long p_flags;

		if (is_64bit) {
			Elf64_Phdr phdr;

			offset = phoff + i * sizeof(phdr);

			pread(mbn, &phdr, sizeof(phdr), offset);
			pwrite(mdt, &phdr, sizeof(phdr), offset);
			p_offset = phdr.p_offset;
			p_filesz = phdr.p_filesz;
			p_flags = phdr.p_flags;
		} else {
			Elf32_Phdr phdr;
			size_t offset;

			offset = phoff + i * sizeof(phdr);

			pread(mbn, &phdr, sizeof(phdr), offset);
			pwrite(mdt, &phdr, sizeof(phdr), offset);
			p_offset = phdr.p_offset;
			p_filesz = phdr.p_filesz;
			p_flags = phdr.p_flags;
		}

		if (!p_filesz)
			continue;

		segment = malloc(p_filesz);

		sprintf(ext, ".b%02d", i);

		n = pread(mbn, segment, p_filesz, p_offset);
		if (n < 0)
			warn("error reading segment %d: %s", i, strerror(errno));
		else  if (n != p_filesz)
			warnx("segment %d is truncated", i);
		bxx = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (bxx < 0)
			warn("failed to open %s", argv[2]);

		write(bxx, segment, p_filesz);

		if (((p_flags >> 24) & 7) == 2 || i == 0)
			write(mdt, segment, p_filesz);

		close(bxx);

		free(segment);
	}

	close(mdt);
	close(mbn);

	return 0;
}
