//
//  main.c
//  z-wave-test
//
//  Created by Alex Skalozub on 1/6/12.
//  Copyright (c) 2012 pieceofsummer@gmail.com. All rights reserved.
//

//-------------------------------for hackbusaeng yungu silsep!!!!-------------------------//
//---------------------------this is car raspberry(z-wave+piCam+gpio)---------------------//
//----------------------for default wifi : 192.168.1.208, for g2 wifi : 192.168.43.12----//
//-----------------------made by leesangjun-----------------------------//

#include <stdio.h>
#include <ZWayLib.h>
#include <ZLogging.h>
#include <errno.h>

#include <stdlib.h>

#include <wiringPi.h>

//#include <opencv2/highgui/highgui_c.h>
//#include <opencv/cv.h>

#define B1 29 //arduino digital pin 0
#define B2 28 //arduino digital pin 1
#define B3 27 //arduino digital pin 2
#define B4 26 //arduino digital pin 3

#define B5 21 //arduino digital pin 4
#define B6 24 //arduino digital pin 8
#define B7 23 //arduino digital pin 6
#define B8 22 //arduino digital pin x


#define C1 0 //arduino analog pin A0
#define C2 1 //arduino analog pin A1
#define C3 2 //arduino analog pin A2
#define C4 3 //arduino analog pin A3
#define C5 4 //arduino analog pin A4
#define C6 5 //arduino analog pin A5
#define C7 6 //arduino digital pin 7

volatile int int_val2 = 0;

// * Pthread Initialization //
pthread_t pthread;

void* observer_function() {

	while (1) {

		if (int_val2 != 0) {
			if (int_val2 == 1) {
				digitalWrite(B1, 1);
				digitalWrite(B2, 0);
				digitalWrite(B3, 0);
				digitalWrite(B4, 0);
				//delay(500);
			} else if (int_val2 == 2) {
				digitalWrite(B1, 0);
				digitalWrite(B2, 1);
				digitalWrite(B3, 0);
				digitalWrite(B4, 0);
			} else if (int_val2 == 3) {
				digitalWrite(B1, 0);
				digitalWrite(B2, 0);
				digitalWrite(B3, 1);
				digitalWrite(B4, 0);
			} else if (int_val2 == 4) {
				digitalWrite(B1, 0);
				digitalWrite(B2, 0);
				digitalWrite(B3, 0);
				digitalWrite(B4, 1);
			}

			if (int_val2 == 5) {
				digitalWrite(B5, 1);
				digitalWrite(B6, 0);
			} else if (int_val2 == 6) {
				digitalWrite(B5, 0);
				digitalWrite(B6, 1);
			}

			if (int_val2 == 7) {
				digitalWrite(B7, 1);
				digitalWrite(B8, 0);
			} else if (int_val2 == 8) {
				digitalWrite(B7, 0);
				digitalWrite(B8, 1);
			}

			if (int_val2 < 30 && int_val2 > 15)
				int_val2 = 30;
			if (int_val2 > 150 && int_val2 < 200)
				int_val2 = 150;

			if (int_val2 >= 30 && int_val2 <= 150) {
				int_val2 -= 30;

				if ((int) (int_val2 / 64) == 1) {
					int_val2 -= 64;
					digitalWrite(C1, 1);
					printf("\n 64 get \n");
				} else {
					digitalWrite(C1, 0);
					printf("\n 64 non get \n");
				}

				if ((int) (int_val2 / 32) == 1) {
					int_val2 -= 32;
					digitalWrite(C2, 1);
				} else {
					digitalWrite(C2, 0);
				}

				if ((int) (int_val2 / 16) == 1) {
					int_val2 -= 16;
					digitalWrite(C3, 1);
				} else {
					digitalWrite(C3, 0);
				}

				if ((int) (int_val2 / 8) == 1) {
					int_val2 -= 8;
					digitalWrite(C4, 1);
				} else {
					digitalWrite(C4, 0);
				}

				if ((int) (int_val2 / 4) == 1) {
					int_val2 -= 4;
					digitalWrite(C5, 1);
				} else {
					digitalWrite(C5, 0);
				}

				if ((int) (int_val2 / 2) == 1) {
					int_val2 -= 2;
					digitalWrite(C6, 1);
				} else {
					digitalWrite(C6, 0);
				}

				if ((int) (int_val2 / 1) == 1) {
					int_val2 -= 1;
					digitalWrite(C7, 1);
				} else {
					digitalWrite(C7, 0);
				}
				//

			}

			//	digitalWrite(B9, 1);
			//	delay(100);
			printf("pthread active : %d \n", int_val2);
			int_val2 = 0;
		} else if (int_val2 == 0) {

		}
	}
}
//////////////////////////////

int do_work(ZWay zway);

void test_memory(ZWay zway) {
	ZWError r;

	ZWBYTE buff1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	r = zway_fc_memory_put_byte(zway, 0x1234, 0x42, NULL, NULL, NULL);
	r = zway_fc_memory_get_byte(zway, 0x1234, NULL, NULL, NULL);
	r = zway_fc_memory_put_buffer(zway, 0x2345, 16, buff1, NULL, NULL, NULL);
	r = zway_fc_memory_get_buffer(zway, 0x2345, 16, NULL, NULL, NULL);
}

void test_save(ZWay zway) {
	ZWError r;

	ZWBYTE *data = NULL;
	size_t length = 0;
	r = zway_controller_config_save(zway, &data, &length);
	if (r == NoError) {
		printf("Successfully saved! (size = %lu)\n",
				(unsigned long int) length);

		char path[MAX_PATH];
		strcpy(path, "/tmp/config.tgz");

		FILE *t = fopen(path, "wb");
		if (t != NULL) {
			fwrite(data, 1, length, t);
			fclose(t);

			printf("Config written to %s\n", path);
		} else {
			printf("Config write error: %s\n", strerror(errno));
		}

		free(data);
	} else {
		printf("Error saving configuration! (err = %s)\n", zstrerror(r));
	}
}

void test_restore(ZWay zway) {
	char path[MAX_PATH];
	strcpy(path, "/tmp/config.tgz");

	printf("Reading config from %s\n", path);

	struct stat finfo;
	int tr = stat(path, &finfo);
	if (tr != 0) {
		if (errno == ENOENT) {
			printf("Config not found. Save one first!\n");
		} else {
			printf("Config read error: %s\n", strerror(errno));
		}
		return;
	}

	size_t length = finfo.st_size;
	ZWBYTE *data = (ZWBYTE*) malloc(length);
	if (data == NULL) {
		printf("Failed to alloc %zu bytes!\n", length);
		return;
	}

	FILE *t = fopen(path, "rb");
	if (t != NULL) {
		fread(data, 1, length, t);
		fclose(t);
	} else {
		free(data);
		printf("Config read error: %s\n", strerror(errno));
		return;
	}

	ZWError r = zway_controller_config_restore(zway, data, length, FALSE);
	if (r == NoError) {
		printf("Configuration successfully restored\n");
	} else {
		printf("Error restoring configuration! (err = %s)\n", zstrerror(r));
	}

	free(data);
}

void dump_data(const ZWay zway, ZDataHolder data) {
	char *path = zdata_get_path(data);
	ZWDataType type;
	zdata_get_type(data, &type);

	ZWBOOL bool_val;
	int int_val;
	float float_val;
	ZWCSTR str_val;
	const ZWBYTE *binary;
	const int *int_arr;
	const float *float_arr;
	const ZWCSTR *str_arr;
	size_t len, i;

	switch (type) {
	case Empty:
		zway_log(zway, Debug, ZSTR("DATA %s = Empty"), path);
		break;
	case Boolean:
		zdata_get_boolean(data, &bool_val);
		if (bool_val)
			zway_log(zway, Debug, ZSTR("DATA %s = True"), path);
		else
			zway_log(zway, Debug, ZSTR("DATA %s = False"), path);
		break;
	case Integer:
		zdata_get_integer(data, &int_val);
		zway_log(zway, Debug, ZSTR("DATA %s = %d (0x%08x)"), path, int_val,
				int_val);
		break;
	case Float:
		zdata_get_float(data, &float_val);
		zway_log(zway, Debug, ZSTR("DATA %s = %f"), path, float_val);
		break;
	case String:
		zdata_get_string(data, &str_val);
		zway_log(zway, Debug, ZSTR("DATA %s = \"%s\""), path, str_val);
		break;
	case Binary:
		zdata_get_binary(data, &binary, &len);
		zway_log(zway, Debug, ZSTR("DATA %s = byte[%d]"), path, len);
		zway_dump(zway, Debug, ZSTR("  "), len, binary);
		break;
	case ArrayOfInteger:
		zdata_get_integer_array(data, &int_arr, &len);
		zway_log(zway, Debug, ZSTR("DATA %s = int[%d]"), path, len);
		for (i = 0; i < len; i++)
			zway_log(zway, Debug, ZSTR("  [%02d] %d"), i, int_arr[i]);
		break;
	case ArrayOfFloat:
		zdata_get_float_array(data, &float_arr, &len);
		zway_log(zway, Debug, ZSTR("DATA %s = float[%d]"), path, len);
		for (i = 0; i < len; i++)
			zway_log(zway, Debug, ZSTR("  [%02d] %f"), i, float_arr[i]);
		break;
	case ArrayOfString:
		zdata_get_string_array(data, &str_arr, &len);
		zway_log(zway, Debug, ZSTR("DATA %s = string[%d]"), path, len);
		for (i = 0; i < len; i++)
			zway_log(zway, Debug, ZSTR("  [%02d] \"%s\""), i, str_arr[i]);
		break;
	}
	free(path);

	ZDataIterator child = zdata_first_child(data);
	while (child != NULL) {
		path = zdata_get_path(child->data);
		zway_log(zway, Debug, ZSTR("CHILD %s"), path);
		free(path);
		child = zdata_next_child(child);
	}
	//printf("intval : %d \n", int_val);
}
/*
 int get_data(const ZWay zway, ZDataHolder data)
 {
 char *path = zdata_get_path(data);
 ZWDataType type;
 zdata_get_type(data, &type);

 ZWBOOL bool_val;
 int int_val=0;
 float float_val;
 ZWCSTR str_val;
 const ZWBYTE *binary;
 const int *int_arr;
 const float *float_arr;
 const ZWCSTR *str_arr;
 size_t len, i;

 switch (type)
 {
 case Empty:
 zway_log(zway, Debug, ZSTR("DATA %s = Empty"), path);
 break;
 case Boolean:
 zdata_get_boolean(data, &bool_val);
 if (bool_val)
 zway_log(zway, Debug, ZSTR("DATA %s = True"), path);
 else
 zway_log(zway, Debug, ZSTR("DATA %s = False"), path);
 break;
 case Integer:
 zdata_get_integer(data, &int_val);
 zway_log(zway, Debug, ZSTR("DATA %s = %d (0x%08x)"), path, int_val, int_val);
 break;
 case Float:
 zdata_get_float(data, &float_val);
 zway_log(zway, Debug, ZSTR("DATA %s = %f"), path, float_val);
 break;
 case String:
 zdata_get_string(data, &str_val);
 zway_log(zway, Debug, ZSTR("DATA %s = \"%s\""), path, str_val);
 break;
 case Binary:
 zdata_get_binary(data, &binary, &len);
 zway_log(zway, Debug, ZSTR("DATA %s = byte[%d]"), path, len);
 zway_dump(zway, Debug, ZSTR("  "), len, binary);
 break;
 case ArrayOfInteger:
 zdata_get_integer_array(data, &int_arr, &len);
 zway_log(zway, Debug, ZSTR("DATA %s = int[%d]"), path, len);
 for (i = 0; i < len; i++)
 zway_log(zway, Debug, ZSTR("  [%02d] %d"), i, int_arr[i]);
 break;
 case ArrayOfFloat:
 zdata_get_float_array(data, &float_arr, &len);
 zway_log(zway, Debug, ZSTR("DATA %s = float[%d]"), path, len);
 for (i = 0; i < len; i++)
 zway_log(zway, Debug, ZSTR("  [%02d] %f"), i, float_arr[i]);
 break;
 case ArrayOfString:
 zdata_get_string_array(data, &str_arr, &len);
 zway_log(zway, Debug, ZSTR("DATA %s = string[%d]"), path, len);
 for (i = 0; i < len; i++)
 zway_log(zway, Debug, ZSTR("  [%02d] \"%s\""), i, str_arr[i]);
 break;
 }
 free(path);

 ZDataIterator child = zdata_first_child(data);
 while (child != NULL)
 {
 path = zdata_get_path(child->data);
 zway_log(zway, Debug, ZSTR("CHILD %s"), path);
 free(path);
 child = zdata_next_child(child);
 }

 printf(" int_val:%d",int_val);
 //return int_val;
 }*/

void print_basic_holder(const ZWay zway, ZWDataChangeType type,
		ZDataHolder data) {
	int int_val;

	zdata_get_integer(data, &int_val);
	zway_log(zway, Debug, ZSTR("Basic set value = %i"), int_val);
	int_val2 = int_val;
	printf("print, int_val : %d \n", int_val2);
}

void print_D_I_CC_event(const ZWay zway, ZWDeviceChangeType type,
		ZWBYTE node_id, ZWBYTE instance_id, ZWBYTE command_id, void *arg) {
	switch (type) {
	case DeviceAdded:
		zway_log(zway, Information, ZSTR("New device added: %i"), node_id);
		break;

	case DeviceRemoved:
		zway_log(zway, Information, ZSTR("Device removed: %i"), node_id);
		break;

	case InstanceAdded:
		zway_log(zway, Information, ZSTR("New instance added to device %i: %i"),
				node_id, instance_id);
		break;

	case InstanceRemoved:
		zway_log(zway, Information, ZSTR("Instance removed from device %i: %i"),
				node_id, instance_id);
		break;

	case CommandAdded:
		zway_log(zway, Information,
				ZSTR("New Command Class added to device %i:%i: %i"), node_id,
				instance_id, command_id);
		break;

	case CommandRemoved:
		zway_log(zway, Information,
				ZSTR("Command Class removed from device %i:%i: %i"), node_id,
				instance_id, command_id);
		break;
	}
}

void print_help(void) {
	printf("=== Test commands ===\n"
			"d r <path>\n"
			"d d <dev> <path>\n"
			"d i <dev> <inst> <path>\n"
			"d c <dev> <inst> <ccId> <path>\n"
			"m Memory test\n"
			"s <dev> <inst> <ccId> (s=set|g=get) <val>\n"
			"a Start AddNodeToNetwork\n"
			"A Stop AddNodeToNetwork\n"
			"e Start RemoveNodeFromNetwork\n"
			"E Stop RemoveNodeFromNetwork\n"
			"S save configuration to tar file\n"
			"R restore configuration from tar file\n"
			"W restore stick from config\n"
			"x exit\n");
}

int do_work(ZWay zway) {
	print_help();

	char cmd, cc_cmd, holder_root;
	ZWBYTE dev, inst, cc, cc_val, nconv;
	char data_path[256];
	char cmd_buffer[256];

	ZWBOOL was_idle = FALSE;

	ZWBOOL basic_level_attached = FALSE;

	int skip = 0;
	int running = TRUE;

	int level = 0;

	/*    while(1){
	 digitalWrite(LED1,1);
	 delay(500);
	 digitalWrite(LED1,0);
	 delay(500);
	 }*/
	while (running) {
		/*    	printf("\n\n\n");
		 zdata_acquire_lock(ZDataRoot(zway));
		 level = get_data(zway, zway_find_device_instance_cc_data(zway, '2', '0', 32, 'level'));
		 zdata_release_lock(ZDataRoot(zway));*/

		/*
		 zdata_acquire_lock(ZDataRoot(zway));
		 level = get_data(zway, zway_find_device_data(zway, '1', 'level'));
		 zdata_release_lock(ZDataRoot(zway));
		 */

		printf("data :::::::    %d", int_val2);

		//zdata_add_callback(zway_find_device_instance_cc_data(zway,1,0,32,"mylevel"),[ZDataChangeCallback] callback, FALSE,[void*]arg);

		printf("\nLOG,while1\n");

		if (!zway_is_running(zway)) //this is almost can not be execute
				{
			running = FALSE;
			break;
		}

		printf("\nLOG,while2\n");
		if (!zway_is_idle(zway)) //maybe wait for
				{
			printf("\nLOG, sleep\n");
			sleep_ms(10);
			continue;
		}
		skip = 0;

		/*      printf("\nLOG,while3\n");
		 if (!basic_level_attached)
		 {
		 printf("\nLOG,basic level start\n");

		 ZDataHolder basic_level_holder;
		 zdata_acquire_lock(ZDataRoot(zway));

		 basic_level_holder = zway_find_device_instance_cc_data(zway, 8, 0, 0x20, "mylevel");
		 //   basic_level_holder = zway_find_device_instance_cc_data(zway, 8, 0, 0x26, "level");
		 if (basic_level_holder)
		 {
		 printf("\nLOG,basic level holder\n");

		 basic_level_attached = (zdata_add_callback(basic_level_holder, (ZDataChangeCallback) print_basic_holder, FALSE, NULL) == NoError);
		 if (basic_level_attached){

		 zway_log(zway, Debug, ZSTR("Basic.data.mylevel holder handler attached to device 8 instance 0"));
		 printf("\nLOG,basic level attatched\n");
		 }

		 }
		 zdata_release_lock(ZDataRoot(zway));

		 }*/

		printf("\nLOG,while4\n");
		if (!basic_level_attached) {
			printf("\nLOG,basic level start\n");

			ZDataHolder basic_level_holder;
			zdata_acquire_lock(ZDataRoot(zway));

			//   basic_level_holder = zway_find_device_instance_cc_data(zway, 8, 0, 0x20, "mylevel");
			basic_level_holder = zway_find_device_instance_cc_data(zway, 2, 0,
					32, "level");
			if (basic_level_holder) {
				printf("\nLOG,basic level holder\n");

				basic_level_attached = (zdata_add_callback(basic_level_holder,
						(ZDataChangeCallback) print_basic_holder, FALSE, NULL)
						== NoError);
				if (basic_level_attached) {

					zway_log(zway, Debug,
							ZSTR(
									"Basic.data.mylevel holder handler attached to device 8 instance 0"));
					printf("\nLOG,basic level attatched\n");

					pthread_create(&pthread, NULL, observer_function, NULL);

					pthread_detach(pthread);
				}
			}
			zdata_release_lock(ZDataRoot(zway));

		}

		printf("> ");
		fgets(cmd_buffer, 255, stdin);
		was_idle = FALSE;

		nconv = sscanf(cmd_buffer, "%c %*s", &cmd);
		printf("nconv : %hhd", &nconv);
		if (nconv > 0) {
			switch (cmd) {
			case 'h':
				print_help();
				break;

			case 'd':
				nconv = sscanf(cmd_buffer, "%c %c", &cmd, &holder_root);
				if (nconv > 1) {
					switch (holder_root) {
					case 'r':
						nconv = sscanf(cmd_buffer, "%c %c %s", &cmd,
								&holder_root, data_path);
						if (nconv >= 2) {
							if (nconv == 2) {
								data_path[0] = '.';
								data_path[1] = '\0';
							}
							zdata_acquire_lock(ZDataRoot(zway));
							dump_data(zway,
									zway_find_controller_data(zway, data_path));
							zdata_release_lock(ZDataRoot(zway));
						}
						break;

					case 'd':
						nconv = sscanf(cmd_buffer, "%c %c %hhd %s", &cmd,
								&holder_root, &dev, data_path);
						if (nconv >= 3) {
							if (nconv == 3) {
								data_path[0] = '.';
								data_path[1] = '\0';
							}
							zdata_acquire_lock(ZDataRoot(zway));
							dump_data(zway,
									zway_find_device_data(zway, dev,
											data_path));
							zdata_release_lock(ZDataRoot(zway));
						}
						break;
					case 'i':
						nconv = sscanf(cmd_buffer, "%c %c %hhd %hhd %s", &cmd,
								&holder_root, &dev, &inst, data_path);
						if (nconv >= 4) {
							if (nconv == 4) {
								data_path[0] = '.';
								data_path[1] = '\0';
							}
							zdata_acquire_lock(ZDataRoot(zway));
							dump_data(zway,
									zway_find_device_instance_data(zway, dev,
											inst, data_path));
							zdata_release_lock(ZDataRoot(zway));
						}
						break;
					case 'c':
						nconv = sscanf(cmd_buffer, "%c %c %hhd %hhd %hhd %s",
								&cmd, &holder_root, &dev, &inst, &cc,
								data_path);
						if (nconv >= 5) {
							if (nconv == 5) {
								data_path[0] = '.';
								data_path[1] = '\0';
							}
							zdata_acquire_lock(ZDataRoot(zway));
							dump_data(zway,
									zway_find_device_instance_cc_data(zway, dev,
											inst, cc, data_path));
							zdata_release_lock(ZDataRoot(zway));
						}
						break;
					}
				}
				break;

			case 's':
				nconv = sscanf(cmd_buffer, "%c %hhd %hhd %hhd %c %hhd", &cmd,
						&dev, &inst, &cc, &cc_cmd, &cc_val);

				printf("nconv : %hhd \n", &nconv);

				/*

				 if (nconv == 6 && cmd == 's' && cc == 0x20 && cc_cmd == 's')
				 {
				 printf("nconv2 : %hhd \n",&nconv);
				 printf("if gogo111111111111111111111111111111111111111111111111111111111111111111\n");
				 zdata_acquire_lock(ZDataRoot(zway));
				 zway_cc_switch_multilevel_set(zway, dev, inst, cc_val, 0x01 ,NULL ,NULL,0);
				 zdata_release_lock(ZDataRoot(zway));
				 }
				 else if (nconv == 5 && cmd == 's' && cc == 0x20 && cc_cmd == 'g')
				 {
				 printf("elseif gogo\n");
				 zdata_acquire_lock(ZDataRoot(zway));
				 zway_cc_switch_multilevel_get(zway, dev, inst,NULL,NULL,0);
				 zdata_release_lock(ZDataRoot(zway));
				 }

				 */

				if (nconv == 6 && cmd == 's' && cc == 0x20 && cc_cmd == 's') {
					printf("nconv2 : %hhd \n", &nconv);
					printf(
							"if gogo111111111111111111111111111111111111111111111111111111111111111111\n");
					zdata_acquire_lock(ZDataRoot(zway));
					zway_cc_basic_set(zway, dev, inst, cc_val, NULL, NULL,
							NULL);
					zdata_release_lock(ZDataRoot(zway));

				} else if (nconv == 5 && cmd == 's' && cc == 0x20
						&& cc_cmd == 'g') {
					printf("elseif gogo\n");
					zdata_acquire_lock(ZDataRoot(zway));
					zway_cc_basic_get(zway, dev, inst, NULL, NULL, NULL);
					zdata_release_lock(ZDataRoot(zway));
				} else if (nconv == 6 && cmd == 's' && cc == 0x26
						&& cc_cmd == 's') {
					printf("nconv2 : %hhd \n", &nconv);
					printf(
							"if gogo333333333333333333333333333333333333333333333333333333333\n");
					zdata_acquire_lock(ZDataRoot(zway));
					zway_cc_switch_multilevel_set(zway, dev, inst, cc_val, 0x01,
							NULL, NULL, 0);
					zdata_release_lock(ZDataRoot(zway));
				}

				break;

			case 'n':
				nconv = sscanf(cmd_buffer, "%c %hhd", &cmd, &dev);

				if (nconv == 2)
					zway_fc_request_node_information(zway, dev, NULL, NULL,
							NULL);
				break;

			case 'm':
				test_memory(zway);
				break;

			case 'a':
				zway_fc_add_node_to_network(zway, TRUE, TRUE, NULL, NULL, NULL);
				break;
			case 'A':
				zway_fc_add_node_to_network(zway, FALSE, TRUE, NULL, NULL,
						NULL);
				break;

			case 'e':
				zway_fc_remove_node_from_network(zway, TRUE, TRUE, NULL, NULL,
						NULL);
				break;

			case 'x':
				running = FALSE;
				break;

			case 'S':
				test_save(zway);
				break;

			case 'R':
				test_restore(zway);
				break;
			case 'q':
				//	digitalWrite(LED1, 1);
				break;
			case 'w':
				//	digitalWrite(LED1, 0);
				break;

			case 'l':
				nconv = sscanf(cmd_buffer, "%c %hhd %hhd", &cmd, &dev, &inst);
				switch (nconv) {
				case 1: {
					ZWDevicesList list = zway_devices_list(zway);
					if (list != NULL) {
						int i = 0;
						printf("Devices list: ");
						while (list[i]) {
							printf("%i ", list[i]);
							i++;
						}
						zway_devices_list_free(list);
						printf("\n");
					} else
						printf("Error happened requesting devices list\n");
				}
					break;

				case 2: {
					ZWInstancesList list = zway_instances_list(zway, dev);
					if (list != NULL) {
						int i = 0;
						printf("Instances list for device %i: ", dev);
						while (list[i]) {
							printf("%i ", list[i]);
							i++;
						}
						zway_instances_list_free(list);
						printf("\n");
					} else
						printf("Error happened requesting instances list\n");
				}
					break;

				case 3: {
					ZWCommandClassesList list = zway_command_classes_list(zway,
							dev, inst);
					if (list != NULL) {
						int i = 0;
						printf(
								"Command Classes list for device %i instance %i: ",
								dev, inst);
						while (list[i]) {
							printf("%02x ", list[i]);
							i++;
						}
						zway_command_classes_list_free(list);
						printf("\n");
					} else
						printf(
								"Error happened requesting command classes list\n");
				}
					break;
				}
				break;
			}
		}
	}

	return 0;
}

void print_zway_terminated(ZWay zway, void* arg) {
	zway_log(zway, Information, ZSTR("Z-Way terminated"));
}

void pin_init() {
	if (wiringPiSetup() == -1) {
		printf("XXXXXXXXXXXX");
	}
	pinMode(B1, OUTPUT);
	pinMode(B2, OUTPUT);

	pinMode(B3, OUTPUT);
	pinMode(B4, OUTPUT);

	pinMode(B5, OUTPUT);
	pinMode(B6, OUTPUT);

	pinMode(B7, OUTPUT);
	pinMode(B8, OUTPUT);

	//pinMode(B9, OUTPUT);

	pinMode(C1, OUTPUT);
	pinMode(C2, OUTPUT);
	pinMode(C3, OUTPUT);
	pinMode(C4, OUTPUT);
	pinMode(C5, OUTPUT);
	pinMode(C6, OUTPUT);
	pinMode(C7, OUTPUT);
//

}

int main(int argc, const char * argv[]) {
	ZWLog logger = zlog_create(stdout, Debug);
//	ZWLog logger = NULL;
	ZWay zway = NULL;
#ifdef _WINDOWS
	ZWError r = zway_init(&zway, ZSTR("COM3"), NULL, NULL, NULL, NULL, logger);
#endif
#ifdef __MACH__
	ZWError r = zway_init(&zway, ZSTR("/dev/cu.SLAB_USBtoUART"), NULL, NULL, NULL, NULL, logger);
#endif
#ifdef __linux__
	// ZWError r = zway_init(&zway, ZSTR("/dev/ttyUSB0"), NULL, NULL, NULL, NULL, logger);
	ZWError r = zway_init(&zway, ZSTR("/dev/ttyAMA0"), NULL, NULL, NULL, NULL,
			logger);
#endif
	if (r != NoError) {
		printf("\n initError \n");
		zway_log_error(zway, Critical, "Failed to init ZWay", r);
		return -1;
	}

	printf("\ncallback\n");
	zway_device_add_callback(zway,
			DeviceAdded | DeviceRemoved | InstanceAdded | InstanceRemoved
					| CommandAdded | CommandRemoved, print_D_I_CC_event, NULL);

	printf("\nstart\n");
	r = zway_start(zway, print_zway_terminated, NULL);
	if (r != NoError) {
		printf("\nstartError\n");
		zway_log_error(zway, Critical, "Failed to start ZWay", r);
		return -1;
	}

	r = zway_discover(zway);
	if (r != NoError) {
		zway_log_error(zway, Critical, "Failed to negotiate with Z-Wave stick",
				r);
		return -1;
	}

	pin_init();

	printf("\ndowork\n");
	// Application code
	int code = do_work(zway);
	//int code =0;

	r = zway_stop(zway);

	zway_terminate(&zway);

	return code;
}

