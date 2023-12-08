char *keyName = input_file_path;
    // char *SkeyName = input_file_path;
    // int dot_position = -1;
    // for (int i = strlen(keyName) - 1; i >= 0; i--) {
    //     if (keyName[i] == '.') {
    //         dot_position = i;
    //         break;
    //     }
    // }

    // if (dot_position == -1) {
    //     printf("Invalid file name.\n");
    //     exit(1);
    // }

    // // Generate file names for P and S arrays
    // strcpy(keyName + dot_position, ".Blowkey.txt");

    // dot_position = -1;
    // for (int i = strlen(SkeyName) - 1; i >= 0; i--) {
    //     if (SkeyName[i] == '.') {
    //         dot_position = i;
    //         break;
    //     }
    // }

    // strcpy(SkeyName + dot_position, ".Skey.txt");

    // FILE *keyOutput = fopen(keyName, "wb");
    // FILE *sKeyOutput = fopen(SkeyName, "wb");

    // if (!keyOutput) {
    //     perror("Error opening key output file");
    //     exit(1);
    // }
    // if (!sKeyOutput) {
    //     perror("Error opening S key output file");
    //     fclose(keyOutput);
    //     exit(1);
    // }

    // // Write the P array to the key output file
    // if (fwrite(P, sizeof(uint32_t), 18, keyOutput) != 18) {
    //     perror("Error writing P array to the key output file");
    //     fclose(keyOutput);
    //     fclose(sKeyOutput);
    //     exit(1);
    // }

    // // Write the S array to the S key output file
    // if (fwrite(S, sizeof(uint32_t), 4 * 256, sKeyOutput) != 4 * 256) {
    //     perror("Error writing S array to the S key output file");
    //     fclose(keyOutput);
    //     fclose(sKeyOutput);
    //     exit(1);
    // }

    // fclose(keyOutput);
    // fclose(sKeyOutput);