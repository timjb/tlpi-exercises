include ../Makefile.inc

GEN_EXE = tee cp_holes append_seek_write atomic_append dup_clone dup_common_attrs \
          readv_writev_clone longjmp_error my_env free_and_sbrk malloc_free_clone \
          getpwnam_clone initgroups_clone tail chmod_a_plus_rX realpath_clone \
          getcwd_clone signal_ign_demo sigaction_nodefer sigaction_resethand \
          abort_clone

LINUX_EXE = large_file2 list_processes_for_user pstree processes_open_file \
          eaccess_clone chattr_clone setfattr_clone acl_perms nftw_statistics \
          inotify_recursive

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

acl_perms : acl_perms.o
	${CC} -o $@ acl_perms.o ${CFLAGS} ${LDLIBS} ${LINUX_LIBACL}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
