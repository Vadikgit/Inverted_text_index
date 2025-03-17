OBJECTS := objects
ALL_LDFLAGS := $(LDFLAGS) $(EXTRA_LDFLAGS)

clean:
	@rm -rf $(OBJECTS)/* main
	@rm -rf $(OBJECTS)

# Directories
$(OBJECTS):
	$(Q)mkdir -p $@

$(OBJECTS)/huffman_code_processor.o: huffman_code_processor.cpp huffman_code_processor.h | $(OBJECTS)
	@$(CXX) $(CXXFLAGS) -g -c huffman_code_processor.cpp -o $@

$(OBJECTS)/posting_list.o: posting_list.cpp posting_list.h | $(OBJECTS)
	@$(CXX) $(CXXFLAGS) -g -c posting_list.cpp -o $@

$(OBJECTS)/inverted_text_index.o: inverted_text_index.cpp inverted_text_index.h | $(OBJECTS)
	@$(CXX) $(CXXFLAGS) -g -c inverted_text_index.cpp -o $@

$(OBJECTS)/main.o: main.cpp inverted_text_index.h | $(OBJECTS)
	@$(CXX) $(CXXFLAGS) -g -c main.cpp -o $@

main: $(OBJECTS)/huffman_code_processor.o $(OBJECTS)/posting_list.o $(OBJECTS)/inverted_text_index.o $(OBJECTS)/main.o | $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $^ $(ALL_LDFLAGS) -g -lelf -lz -o $@

.DELETE_ON_ERROR:

.SECONDARY:
