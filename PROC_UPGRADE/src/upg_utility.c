#include <stdio.h>
#include <mtd/mtd-user.h>

#include "upg_utility.h"
#include "upg_download.h"
#include <string.h>

/*-------------------------------------------------------------------------------------
                    for zip header info -begin
-------------------------------------------------------------------------------------*/

void parse_zip_head_info(const ZIP_HEADER_RAW *raw_data, ZIP_HEADER *data) {
    data->sign_head = *(int32 *) (&raw_data->sign_head);
    data->version = *(int16 *) (&raw_data->version);
    data->gp_bit_flag = *(int16 *) (&raw_data->gp_bit_flag);
    data->comp_method = *(int16 *) (&raw_data->comp_method);
    data->mod_time = *(int16 *) (&raw_data->mod_time);
    data->mod_date = *(int16 *) (&raw_data->mod_date);
    data->crc32 = *(int32 *) (&raw_data->crc32);
    data->comp_size = *(int32 *) (&raw_data->comp_size);
    data->uncomp_size = *(int32 *) (&raw_data->uncomp_size);
    data->name_length = *(int16 *) (&raw_data->name_length);
    data->ext_field_length = *(int16 *) (&raw_data->ext_field_length);
    strncpy(data->file_name, (char *) (&raw_data->file_name), data->name_length);


    printf(("Got head info:\n"));

    printf("\t sign:\t\t%d\n", data->sign_head);
    printf("\t version:\t\t%x\n", data->version);
    printf("\t gp_bit_flag:\t%x\n", data->gp_bit_flag);
    printf("\t comp_method:\t%x\n", data->comp_method);
    printf("\t comp_size:\t\t%u\n", data->comp_size);
    printf("\t uncomp_size:\t%d\n", data->uncomp_size);
    printf("\t name_len:\t\t%d\n", data->name_length);
    printf("\t ext_field_length:%d\n", data->ext_field_length);
    printf("\t file_name:\t\t%s\n", data->file_name);

//#endif
}

/*-------------------------------------------------------------------------------------
                    for zip header info -end
-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
                    for image partition info -begin
-------------------------------------------------------------------------------------*/
#if CONFIG_UPGRADE_USE_NAND_FLASH

int32 get_partition_blk_size_by_name(const char *img_name, int32 *blk_size) {
    FILE *fp = NULL;
    int32 i4_ret = -1;
    int32 match_num = 0;
    int32 mtd_num = -1;
    int32 mtd_size = 0;
    int32 mtd_erase_size = 0;
    char mtd_name[64] = {0};
    //char  mtd_name[64];
    char buf[128] = {0};

    fp = fopen(PROC_MTD_PATH, "rb");
    while (fgets(buf, sizeof(buf), fp)) {
        match_num = sscanf(buf, "mtd%d: %x %x \"%63[^\"]", &mtd_num, &mtd_size, &mtd_erase_size, mtd_name);
        if ((match_num == 4) && !strncmp(mtd_name, img_name, strlen(img_name))) {
            i4_ret = 0;
            printf("mtd name:%s, mtd num:%d, mtd_erase_size=%d\n", mtd_name, mtd_num, mtd_erase_size);
            break;
        }
    }
    if (i4_ret) {
        fclose(fp);
        printf("get_partition_blk_size_by_name get %s mtd number failed\n", img_name);
        return i4_ret;
    }
    *blk_size = mtd_erase_size;
    fclose(fp);

    return i4_ret;
}

#else /* CONFIG_UPGRADE_USE_NAND_FLASH */
int32 get_partition_blk_size_by_name(const char* img_name, int32* blk_size)
{
    printf(("it is not compatible with EMMC now\n"));

    /* TODO: fix it to be compatible with EMMC */
    /* though most emmc physic block size is 512, it is not partition block size */
    *blk_size = 512;
    return 0;
}
#endif /* CONFIG_UPGRADE_USE_NAND_FLASH */
/*-------------------------------------------------------------------------------------
                    for image partition info -end
-------------------------------------------------------------------------------------*/

