export FM_LASTD='/tmp/fm_lastd'

fm () {
	/home/user/code/c_cpp/c_fm/v3/a.out && cd $(cat $FM_LASTD)
}

