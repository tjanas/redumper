module;
#include <format>
#include <list>
#include <memory>
#include <string>

export module options;

import utils.logger;
import utils.misc;



namespace gpsxre
{

export struct Options
{
	std::string command_line;

	std::list<std::string> commands;

	bool help;
	bool version;
	bool verbose;
	bool debug;

	std::string image_path;
	std::string image_name;
	bool overwrite;
	bool force_split;
	bool leave_unchanged;

	std::string drive;
	std::unique_ptr<std::string> drive_type;
	std::unique_ptr<int> drive_read_offset;
	std::unique_ptr<int> drive_c2_shift;
	std::unique_ptr<int> drive_pregap_start;
	std::unique_ptr<std::string> drive_read_method;
	std::unique_ptr<std::string> drive_sector_order;
	std::unique_ptr<int> speed;
	int retries;
	bool refine_subchannel;
	std::unique_ptr<int> lba_start;
	std::unique_ptr<int> lba_end;
	bool force_qtoc;
	std::string skip;
	int skip_fill;
	bool iso9660_trim;
	bool plextor_leadin_skip;
	int plextor_leadin_retries;
	bool asus_skip_leadout;
	bool disable_cdtext;
	bool correct_offset_shift;
	bool offset_shift_relocate;
	std::unique_ptr<int> force_offset;
	int audio_silence_threshold;


	Options(int argc, const char *argv[])
		: help(false)
		, version(false)
		, verbose(false)
		, debug(false)
		, overwrite(false)
		, force_split(false)
		, leave_unchanged(false)
		, retries(0)
		, refine_subchannel(false)
		, force_qtoc(false)
		, skip_fill(0x55)
		, iso9660_trim(false)
		, plextor_leadin_skip(false)
		, plextor_leadin_retries(4)
		, asus_skip_leadout(false)
		, disable_cdtext(false)
		, correct_offset_shift(false)
		, offset_shift_relocate(false)
		, audio_silence_threshold(32)
	{
		for(int i = 0; i < argc; ++i)
		{
			std::string argument = argv[i];

			bool quoted = false;
			if(argument.find(' ') != std::string::npos)
				quoted = true;

			command_line += std::format("{}{}{}{}", quoted ? "\"" : "", argument, quoted ? "\"" : "", i + 1 == argc ? "" : " ");
		}

		std::string *s_value = nullptr;
		int *i_value = nullptr;
		for(int i = 1; i < argc; ++i)
		{
			std::string o(argv[i]);

			// option
			if(o[0] == '-')
			{
				std::string key;
				auto value_pos = o.find("=");
				if(value_pos == std::string::npos)
				{
					key = o;
					o.clear();
				}
				else
				{
					key = std::string(o, 0, value_pos);
					o = std::string(o, value_pos + 1);
				}

				if(s_value == nullptr && i_value == nullptr)
				{
					if(key == "--help" || key == "-h")
						help = true;
					else if(key == "--version")
						version = true;
					else if(key == "--verbose")
						verbose = true;
					else if(key == "--debug")
						debug = true;
					else if(key == "--image-path")
						s_value = &image_path;
					else if(key == "--image-name")
						s_value = &image_name;
					else if(key == "--overwrite")
						overwrite = true;
					else if(key == "--force-split")
						force_split = true;
					else if(key == "--leave-unchanged")
						leave_unchanged = true;
					else if(key == "--drive")
						s_value = &drive;
					else if(key == "--drive-type")
					{
						drive_type = std::make_unique<std::string>();
						s_value = drive_type.get();
					}
					else if(key == "--drive-read-offset")
					{
						drive_read_offset = std::make_unique<int>();
						i_value = drive_read_offset.get();
					}
					else if(key == "--drive-c2-shift")
					{
						drive_c2_shift = std::make_unique<int>();
						i_value = drive_c2_shift.get();
					}
					else if(key == "--drive-pregap-start")
					{
						drive_pregap_start = std::make_unique<int>();
						i_value = drive_pregap_start.get();
					}
					else if(key == "--drive-read-method")
					{
						drive_read_method = std::make_unique<std::string>();
						s_value = drive_read_method.get();
					}
					else if(key == "--drive-sector-order")
					{
						drive_sector_order = std::make_unique<std::string>();
						s_value = drive_sector_order.get();
					}
					else if(key == "--speed")
					{
						speed = std::make_unique<int>();
						i_value = speed.get();
					}
					else if(key == "--retries")
						i_value = &retries;
					else if(key == "--refine-subchannel")
						refine_subchannel = true;
					else if(key == "--lba-start")
					{
						lba_start = std::make_unique<int>();
						i_value = lba_start.get();
					}
					else if(key == "--lba-end")
					{
						lba_end = std::make_unique<int>();
						i_value = lba_end.get();
					}
					else if(key == "--force-qtoc")
						force_qtoc = true;
					else if(key == "--skip")
						s_value = &skip;
					else if(key == "--skip-fill")
						i_value = &skip_fill;
					else if(key == "--iso9660-trim")
						iso9660_trim = true;
					else if(key == "--plextor-leadin-skip")
						plextor_leadin_skip = true;
					else if(key == "--plextor-leadin-retries")
						i_value = &plextor_leadin_retries;
					else if(key == "--asus-skip-leadout")
						asus_skip_leadout = true;
					else if(key == "--disable-cdtext")
						disable_cdtext = true;
					else if(key == "--correct-offset-shift")
						correct_offset_shift = true;
					else if(key == "--offset-shift-relocate")
						offset_shift_relocate = true;
					else if(key == "--force-offset")
					{
						force_offset = std::make_unique<int>();
						i_value = force_offset.get();
					}
					else if(key == "--audio-silence-threshold")
						i_value = &audio_silence_threshold;
					// unknown option
					else
					{
						throw_line("unknown option ({})", key);
					}
				}
				else
					throw_line("option value expected ({})", argv[i - 1]);
			}

			if(!o.empty())
			{
				if(s_value != nullptr)
				{
					*s_value = o;
					s_value = nullptr;
				}
				else if(i_value != nullptr)
				{
					*i_value = stoll_strict(o);
					i_value = nullptr;
				}
				else
					commands.emplace_back(o);
			}
		}
	}


	void printUsage()
	{
		LOG("usage: redumper [command] [options]");
		LOG("");

		LOG("COMMANDS:");
		LOG("\tcd        \taggregate mode {{dump => protection => refine => split => info}} (default)");
		LOG("\tdump      \tdumps disc to primary dump files");
		LOG("\tprotection\tscans dump files for protections");
		LOG("\trefine    \trefines dump files by re-reading the disc");
		LOG("\tsplit     \tgenerates BIN/CUE track split from dump files");
		LOG("\tinfo      \toutputs basic information from BIN/CUE track split");
		LOG("");

		LOG("OPTIONS:");
		LOG("\t(general)");
		LOG("\t--help,-h                      \tprint usage");
		LOG("\t--version                      \tprint version");
		LOG("\t--verbose                      \tverbose output");
		LOG("\t--drive=VALUE                  \tdrive to use, first available drive with disc, if not provided");
		LOG("\t--speed=VALUE                  \tdrive read speed, optimal drive speed will be used if not provided");
		LOG("\t--retries=VALUE                \tnumber of sector retries in case of SCSI/C2 error (default: {})", retries);
		LOG("\t--image-path=VALUE             \tdump files base directory");
		LOG("\t--image-name=VALUE             \tdump files prefix, autogenerated in dump mode if not provided");
		LOG("\t--overwrite                    \toverwrites previously generated dump files");
		LOG("");
		LOG("\t(drive configuration)");
		LOG("\t--drive-type=VALUE             \toverride drive type, possible values: GENERIC, PLEXTOR, LG_ASUS");
		LOG("\t--drive-read-offset=VALUE      \toverride drive read offset");
		LOG("\t--drive-c2-shift=VALUE         \toverride drive C2 shift");
		LOG("\t--drive-pregap-start=VALUE     \toverride drive pre-gap start LBA");
		LOG("\t--drive-read-method=VALUE      \toverride drive read method, possible values: BE, D8, BE_CDDA");
		LOG("\t--drive-sector-order=VALUE     \toverride drive sector order, possible values: DATA_C2_SUB, DATA_SUB_C2");
		LOG("");
		LOG("\t(drive specific)");
		LOG("\t--plextor-leadin-skip          \tskip dumping lead-in using negative range");
		LOG("\t--plextor-leadin-retries=VALUE \tmaximum number of lead-in retries per session (default: {})", plextor_leadin_retries);
		LOG("\t--asus-skip-leadout            \tskip extracting lead-out from drive cache");
		LOG("\t--disable-cdtext               \tdisable CD-TEXT reading");
		LOG("");
		LOG("\t(offset)");
		LOG("\t--force-offset=VALUE           \toverride offset autodetection and use supplied value");
		LOG("\t--audio-silence-threshold=VALUE\tmaximum absolute sample value to treat it as silence (default: {})", audio_silence_threshold);
		LOG("\t--correct-offset-shift         \tcorrect disc write offset shift");
		LOG("\t--offset-shift-relocate        \tdon't merge offset groups with non-matching LBA");
		LOG("");
		LOG("\t(split)");
		LOG("\t--force-split                  \tforce track split with errors");
		LOG("\t--leave-unchanged              \tdon't replace erroneous sectors with generated ones");
		LOG("\t--force-qtoc                   \tForce QTOC based track split");
		LOG("\t--skip-fill=VALUE              \tfill byte value for skipped sectors (default: 0x{:02X})", skip_fill);
		LOG("\t--iso9660-trim                 \ttrim each ISO9660 data track to PVD volume size, useful for discs with fake TOC");
		LOG("");
		LOG("\t(miscellaneous)");
		LOG("\t--lba-start=VALUE              \tLBA to start dumping from");
		LOG("\t--lba-end=VALUE                \tLBA to stop dumping at (everything before the value), useful for discs with fake TOC");
		LOG("\t--refine-subchannel            \tIn addition to SCSI/C2, refine subchannel");
		LOG("\t--skip=VALUE                   \tLBA ranges of sectors to skip");
	}
};

}
