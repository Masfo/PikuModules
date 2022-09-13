// AUTOMATICALLY GENERATED BY BUILDINC v0.1.100 TOOL
// LAST BUILD: 2022-09-13 20:08:03

#pragma once

#ifndef PIKUMODULESVERSION_BUILDNUMBER_HEADER_H
#define PIKUMODULESVERSION_BUILDNUMBER_HEADER_H

#include <cstdint>

namespace PikuModulesVersion
{
		 // You can modify major and minor
		constexpr uint32_t major = 0;
		constexpr uint32_t minor = 1;

		 // Do not modify these
		constexpr uint32_t build = 458;
		
		constexpr uint32_t version = major * 10000 + minor * 1000 + build;
		constexpr uint64_t random_seed = 0x78ba5fa8b86f9d38;
		
		constexpr char version_string[] = "v0.1.458";
		constexpr char build_time_string[] = "2022-09-13 20:08:03";
		constexpr char phrase[] = "deafness-italics-ravine";
		constexpr char calver[] = "2022.37.458";

		// Copy paste to import to your project
		/*
			constexpr auto major = PikuModulesVersion::major;
			constexpr auto minor = PikuModulesVersion::minor;
			constexpr auto build = PikuModulesVersion::build;
			constexpr auto version = PikuModulesVersion::version;
			constexpr auto random_seed = PikuModulesVersion::random_seed;
			
			constexpr auto version_string = PikuModulesVersion::version_string;
			constexpr auto build_time_string = PikuModulesVersion::build_time_string;
			constexpr auto phrase = PikuModulesVersion::phrase;
			constexpr auto calver = PikuModulesVersion::calver;
		*/
}
#endif // PIKUMODULESVERSION_BUILDNUMBER_HEADER_H

