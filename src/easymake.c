#include "easymake.h"
#include "json.h"

float VERSION = 0.1;

char *easymake_read_file(char *file_path)
{
  FILE *file;
  file = fopen(file_path, "r");
  
  if(!file)
  {
    printf("easymake: build file not found!\n");
    return NULL;
  }
  
  fseek(file, 0, SEEK_END);
  
  long length = ftell(file);
  rewind(file);
  
  char *text = (char *)malloc(length + 1);
  
  size_t read_count = fread(text, 1, length, file);
  text[read_count] = '\0';
  
  fclose(file);
  return text;
}

BuildOptions easymake_build_options(char *buf)
{
  BuildOptions boptions;
  
  struct json_value_s *root = json_parse(buf, strlen(buf));
  
  if(root->type == json_type_object)
  {
    struct json_object_s *object = (struct json_object_s *)root->payload;
    
    struct json_object_element_s *key = object->start;
    
    while(key != NULL)
    {
      struct json_string_s *str = key->name;
      
      if(!strcmp(str->string, "project"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val->payload;
          boptions.project = strval->string;
        }
      }
      else if(!strcmp(str->string, "compiler"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val->payload;
          boptions.compiler = strval->string;
        }
      }
      else if(!strcmp(str->string, "output"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val->payload;
          boptions.output = strval->string;
        }
      }
      else if(!strcmp(str->string, "sources"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_array)
        {
          struct json_array_s *array = (struct json_array_s *)val->payload;
          
          char **sources = (char **)malloc(sizeof(char *) * array->length);
          
          struct json_array_element_s *arrel = array->start;
          
          int i = 0;
          while(arrel != NULL)
          {
            struct json_value_s *arrval = arrel->value;
            
            if(arrval->type == json_type_string)
            {
              struct json_string_s *arrstr = (struct json_string_s *)arrval->payload;
              sources[i] = (char *)arrstr->string;
            }
            
            if(arrel->next == NULL) break;
            
            i++;
            arrel = arrel->next;
          }
          
          boptions.sources = (const char **)sources;
          boptions.sources_count = array->length;
        }
      }
      else if(!strcmp(str->string, "includes"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_array)
        {
          struct json_array_s *array = (struct json_array_s *)val->payload;
          
          char **includes = (char **)malloc(sizeof(char *) * array->length);
          
          struct json_array_element_s *arrel = array->start;
          
          int i = 0;
          while(arrel != NULL)
          {
            struct json_value_s *arrval = arrel->value;
            
            if(arrval->type == json_type_string)
            {
              struct json_string_s *arrstr = (struct json_string_s *)arrval->payload;
              includes[i] = (char *)arrstr->string;
            }
            
            if(arrel->next == NULL) break;
            
            i++;
            arrel = arrel->next;
          }
          
          boptions.includes = (const char **)includes;
          boptions.includes_count = array->length;
        }
      }
      else if(!strcmp(str->string, "libraries"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_array)
        {
          struct json_array_s *array = (struct json_array_s *)val->payload;
          
          char **libraries = (char **)malloc(sizeof(char *) * array->length);
          
          struct json_array_element_s *arrel = array->start;
          
          int i = 0;
          while(arrel != NULL)
          {
            struct json_value_s *arrval = arrel->value;
            
            if(arrval->type == json_type_string)
            {
              struct json_string_s *arrstr = (struct json_string_s *)arrval->payload;
              libraries[i] = (char *)arrstr->string;
            }
            
            if(arrel->next == NULL) break;
            
            i++;
            arrel = arrel->next;
          }
          
          boptions.libraries = (const char **)libraries;
          boptions.libraries_count = array->length;
        }
      }
      else if(!strcmp(str->string, "compiler_options"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_array)
        {
          struct json_array_s *array = (struct json_array_s *)val->payload;
          
          char **compiler_options = (char **)malloc(sizeof(char *) * array->length);
          
          struct json_array_element_s *arrel = array->start;
          
          int i = 0;
          while(arrel != NULL)
          {
            struct json_value_s *arrval = arrel->value;
            
            if(arrval->type == json_type_string)
            {
              struct json_string_s *arrstr = (struct json_string_s *)arrval->payload;
              compiler_options[i] = (char *)arrstr->string;
            }
            
            if(arrel->next == NULL) break;
            
            i++;
            arrel = arrel->next;
          }
          
          boptions.compiler_options = (const char **)compiler_options;
          boptions.compiler_options_count = array->length;
        }
      }
      
      key = key->next;
    }
  }
  else
  {
    printf("easymake: invalid json!\n");
  }
  
  free(buf);
  return boptions;
}

void easymake_build_project(BuildOptions *boptions)
{
  printf("easymake: building project \'%s\' using compiler \'%s\'\n", boptions->project, boptions->compiler);
  
  char command[256];
  char *temp = "";
  
  if(!boptions->compiler)
  {
    printf("easymake: no compiler specified!\n");
    return;
  }
  
  temp = concat(command, boptions->compiler);
  strcpy(command, temp);
  free(temp);
  
  if(boptions->output)
  {
    temp = concat(command, " -o ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, boptions->output);
    strcpy(command, temp);
    free(temp);
  }
  
  int i;
  
  if(boptions->sources)
  for(i = 0; i < boptions->sources_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->sources)[i]);
    strcpy(command, temp);
    free(temp);
  }
  else
  {
    printf("easymake: no source files specified!\n");
    return;
  }
  
  if(boptions->includes)
  for(i = 0; i < boptions->includes_count; i++)
  {
    temp = concat(command, " -I");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->includes)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  if(boptions->libraries)
  for(i = 0; i < boptions->libraries_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->libraries)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  if(boptions->compiler_options)
  for(i = 0; i < boptions->compiler_options_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->compiler_options)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  system(command);
  
  printf("easymake: build process complete. output file: \'%s\'\n", boptions->output);
}

int main(int argc, char *argv[])
{
  if(argc > 1)
  {
    if(!strcmp(argv[1], "-version"))
    {
      printf("easymake v%.1f - by undersquire\n", VERSION);
    }
    else if(!strcmp(argv[1], "-help"
    {
      printf("usage: \n -version: shows current version \n -help: This page \n -Docs: how to use \n); 
    }
    else if(!strcmp(argv[1], "-Docs"
    {
      printf("\nDocs:\n\nHow to run Easy Make !\n\neasymake FileName.ezmk\n\nOr just easymake if you have a build.ezmk file \n\n\n\nMaking a Easy Make file\n\n\nName your Project!\n\"project\": \"my_project\"\n\n\nNext specify your compiler.\n\"compiler\":\"gcc\"\n\n\nNow place the output!\n\"output\": \"bin/a.out\"\n\n\nAdd all your project sources!\n\"sources\": [\"some/src/*.c,some/src/specific.c\"]\n\n\nIf you have any Includes add them!\n\"includes\": [\"some/directory/all/files/in/here/*.h\",\"some/specific/file.h\"]\n\n\nInclude any libaries\n\"libraries \": [ \"libsomething.a\", \"libidk.so\", \"etc.dll\" ]\n\n\nAdd any compiler flags here!\n\"compiler_options\": [\"-s\",\"-O3\"]\n\n\n\n\nNote: make sure to enclose the entire thing in a bracket \n"); //@TODO: add wiki page link instead
    }
    else
    {
      char *buf = easymake_read_file(argv[1]);
      
      if(!buf) return 0;
      
      BuildOptions boptions = easymake_build_options(buf);
      easymake_build_project(&boptions);
    }
  }
  else
  {
    char *buf = easymake_read_file("build.ezmk");
    
    if(!buf) return 0;
    
    BuildOptions boptions = easymake_build_options(buf);
    easymake_build_project(&boptions);
  }
  
  return 0;
}
