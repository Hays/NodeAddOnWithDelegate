{
  "targets": [
    {
      "target_name": "demoaddon",
      "sources": [ "src/main.cc" ],
      "link_settings": {
          "conditions":[
              ['OS=="mac"', {
                  "libraries": [
                      'Foundation.framework',
                      'Cocoa.framework',
                      '-framework Fake'
                  ],
                  "mac_framework_dirs": [
                      "../Framework",
                  ],
              }
          ]]
      },
      "xcode_settings": {
          "OTHER_CFLAGS": [
              "-x objective-c++ -stdlib=libc++"
          ]
      },
    }
  ]
}